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
#include "ssl_wrapper.h"

static void ev_handler(struct ns_connection *nc, enum ns_event ev, void *p) {
  struct ssl_wrapper_config *config =
    (struct ssl_wrapper_config *) nc->server->server_data;
  struct ns_connection *pc = (struct ns_connection *) nc->connection_data;
  struct iobuf *io = &nc->recv_iobuf;

  (void) p;
  switch (ev) {
    case NS_ACCEPT:
      // New SSL connection. Create a connection to the target, and link them
      nc->connection_data = ns_connect(nc->server,
                                       config->resolved_target_ip,
                                       config->target_port,
                                       config->target_uses_ssl,
                                       nc);
      if (nc->connection_data == NULL) {
        nc->flags |= NSF_CLOSE_IMMEDIATELY;
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
      break;

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

void *ssl_wrapper_init(struct ssl_wrapper_config *config,
                       const char **err_msg) {
  struct ns_server *server = NULL;

  *err_msg = NULL;

  if ((server = (struct ns_server *) calloc(1, sizeof(*server))) == NULL) {
    *err_msg = "malloc failed";
  } else {
    ns_server_init(server, config, ev_handler);

    if (ns_bind(server, config->listening_port) < 0) {
      *err_msg = "ns_bind() failed: bad listening_port";
    }

    if (!resolve(config->target_host, config->resolved_target_ip,
                        sizeof(config->resolved_target_ip))) {
      *err_msg = "resolve() failed: bad target_host";
    }

    if (config->ssl_cert != NULL &&
        ns_set_ssl_cert(server, config->ssl_cert) != 0) {
      *err_msg = "ns_set_ssl_cert() failed: bad server certificate";
    }

    if (config->ssl_ca_cert != NULL &&
        ns_set_ssl_ca_cert(server, config->ssl_ca_cert) != 0) {
      *err_msg = "ns_set_ssl_ca_cert() failed: bad client certificate";
    }

    if (*err_msg != NULL) {
      ns_server_free(server);
      free(server);
      server = NULL;
    }
  }

  return server;
}

void ssl_wrapper_serve(void *param, volatile int *quit) {
  struct ns_server *server = (struct ns_server *) param;

  while (*quit == 0) {
    ns_server_poll(server, 1000);
  }
  ns_server_free(server);
  free(server);
}

#ifndef SSL_WRAPPER_USE_AS_LIBRARY
static int s_received_signal = 0;

static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_received_signal = sig_num;
}

static void show_usage_and_exit(const char *prog) {
  fprintf(stderr, "Usage: %s [OPTIONS]\n"
    "Available options are: \n"
    "  -l <port>  Listening port\n"
    "  -t <host>  Target host\n"
    "  -p <port>  Target port\n"
    "  -S         Target uses SSL (by default, no)\n"
    "  -s <file>  Enable SSL listening with this server sertificate PEM file\n"
    "  -c <file>  Use SSL two-way auth with this CA certificate PEM file\n",
    prog);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  void *wrapper;
  const char *err_msg;
  struct ssl_wrapper_config config;
  int i;

  // Parse command line options
  memset(&config, 0, sizeof(config));
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
      config.listening_port = argv[++i];
    } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
      config.target_host = argv[++i];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      config.target_port = atoi(argv[++i]);
    } else if (strcmp(argv[i], "-S") == 0) {
      config.target_uses_ssl = 1;
    } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
      config.ssl_cert = argv[++i];
    } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      config.ssl_ca_cert = argv[++i];
    } else {
      show_usage_and_exit(argv[0]);
    }
  }

  if (config.listening_port == NULL || config.target_host == NULL ||
      config.target_port <= 0) {
    fprintf(stderr, "%s\n", "Options -l, -t, -p are mandatory");
    exit(EXIT_FAILURE);
  }

  // Setup signal handlers
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);

  if ((wrapper = ssl_wrapper_init(&config, &err_msg)) == NULL) {
    fprintf(stderr, "Error: %s\n", err_msg);
    exit(EXIT_FAILURE);
  }
  ssl_wrapper_serve(wrapper, &s_received_signal);

  return EXIT_SUCCESS;
}
#endif // SSL_WRAPPER_USE_AS_LIBRARY
