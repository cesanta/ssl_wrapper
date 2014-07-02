// Copyright (c) 2014 Cesanta Software Limited
// All rights reserved
//
// This software is dual-licensed: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation. For the terms of this
// license, see <http://www.gnu.org/licenses/>.
//
// You are free to use this software under the terms of the GNU General
// Public License, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// Alternatively, you can license this software under a commercial
// license, as set out in <http://cesanta.com/products.html>.

#include "net_skeleton.h"

static char s_server_host[100];       // SERVER host, e.g. "google.com"
static unsigned short s_server_port;  // SERVER port
static char s_wrapper_host[100];      // WRAPPER_SERVER host
static unsigned short s_wrapper_port; // WRAPPER_SERVER port
static char s_plain_port[10];         // Listening non-SSL port
static char s_ssl_port[10];           // Listening SSL port
static const char *s_hexdump_file;    // Traffic dump file
static const char *s_server_cert = "server.pem";
static const char *s_client_cert = "client.pem";

static void elog(int do_exit, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fputc('\n', stderr);
  if (do_exit) exit(EXIT_FAILURE);
}

static void show_usage_and_exit(const char *prog) {
  elog(1, "Usage: %s [OPTIONS]\n"
       "Available options are: \n"
       "  -c    plain_listening_port:ssl_target_host:ssl_target_port\n"
       "  -s    ssl_listening_port:plain_taget_host:plain_target_port\n"
       "  -cs   client_certficate_pem_file\n"
       "  -ss   server_certificate_pem_file\n"
       "  -hex  debug_hexdump_file", prog);
  exit(EXIT_FAILURE);
}

static void *serving_thread_func(void *param) {
  for (;;) {
    ns_server_poll((struct ns_server *) param, 1000);
  }
  return NULL;
}

static void ev_handler(struct ns_connection *nc, enum ns_event ev, void *p) {
  struct ns_connection *pc = (struct ns_connection *) nc->connection_data;
  struct iobuf *io = &nc->recv_iobuf;

  switch (ev) {
    case NS_ACCEPT:
      // New SSL connection. Create a connection to the target, and link them
      nc->connection_data = ns_connect(nc->server,
        nc->ssl == NULL ? s_wrapper_host : s_server_host,
        nc->ssl == NULL ? s_wrapper_port : s_server_port,
        nc->ssl == NULL ? 1 : 0,
        nc);
      if (nc->connection_data == NULL) {
        nc->flags |= NSF_CLOSE_IMMEDIATELY;
      }
      break;
    case NS_CONNECT:
      // Connection to the target finished. If failed, close both
      if (* (int *) p != 0 && pc != NULL) {
        pc->flags |= NSF_CLOSE_IMMEDIATELY;
      }
      break;
    case NS_CLOSE:
      // If either connection closes, unlink them and shedule closing
      if (pc != NULL) {
        pc->flags |= NSF_FINISHED_SENDING_DATA;
        pc->connection_data = NULL;
      }
      nc->connection_data = NULL;
      break;
    case NS_RECV:
      // Forward arrived data to the other connection, and discard from buffer
      if (pc != NULL) {
        ns_send(pc, io->buf, io->len);
        iobuf_remove(io, io->len);
      }
    default:
      break;
  }
}

// Resolve FDQN "host", store IP address in the "ip". Return 0 on failure.
static int resolve(const char *host, char *ip, size_t ip_len) {
  struct hostent *he;
  return (he = gethostbyname(host)) != NULL &&
    snprintf(ip, ip_len, "%s",
             inet_ntoa(* (struct in_addr *) he->h_addr_list[0]));
}

int main(int argc, char *argv[]) {
  static const char *arg_format = "%9[0-9]:%99[a-zA-Z0-9._-]:%hu";
  struct ns_server plain_server, ssl_server;
  int i;

  // Parse command line options
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      if (sscanf(argv[++i], arg_format,
          s_plain_port, s_wrapper_host, &s_wrapper_port) != 3) {
        show_usage_and_exit(argv[0]);
      }
    } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
      if (sscanf(argv[++i], arg_format,
          s_ssl_port, s_server_host, &s_server_port) != 3) {
        show_usage_and_exit(argv[0]);
      }
    } else if (strcmp(argv[i], "-cc") == 0 && i + 1 < argc) {
      s_client_cert = argv[++i];
    } else if (strcmp(argv[i], "-sc") == 0 && i + 1 < argc) {
      s_server_cert = argv[++i];
    } else if (strcmp(argv[i], "-hex") == 0 && i + 1 < argc) {
      s_hexdump_file = argv[++i];
    } else {
      show_usage_and_exit(argv[0]);
    }
  }

  ns_server_init(&plain_server, NULL, NULL);
  ns_server_init(&ssl_server, NULL, NULL);

  if (s_wrapper_host[0] != '\0') {
    ns_server_init(&ssl_server, NULL, ev_handler);
    if (ns_bind(&ssl_server, s_plain_port) < 0) {
      elog(1, "Error binding to %s", s_ssl_port);
    } else if (!resolve(s_wrapper_host, s_wrapper_host,
               sizeof(s_wrapper_host))) {
      elog(1, "Cannot resolve [%s]", s_wrapper_host);
    }
    elog(0, "TCP --> Port %s <== SSL ==> %s:%hu", s_plain_port, s_wrapper_host,
         s_wrapper_port);
  }

  if (s_server_host[0] != '\0') {
    ns_server_init(&plain_server, NULL, ev_handler);
    if (ns_bind(&plain_server, s_ssl_port) < 0) {
      elog(1, "Error binding to %s", s_ssl_port);
    } else if (ns_set_ssl_cert(&plain_server, s_server_cert) != 0) {
      elog(1, "%s", "SSL certificate error");
    } else if (!resolve(s_server_host, s_server_host, sizeof(s_server_host))) {
      elog(1, "Cannot resolve [%s]", s_server_host);
    }
    elog(0, "SSL ==> Port %s <-- TCP --> %s:%hu", s_ssl_port, s_server_host,
         s_server_port);
  }

  if (s_server_host[0] == '\0' && s_wrapper_host[0] == '\0') {
    show_usage_and_exit(argv[0]);
  }

  ns_start_thread(serving_thread_func, &ssl_server);
  serving_thread_func(&plain_server);

  return EXIT_SUCCESS;
}