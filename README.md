ssl_wrapper
===========

SSL Wrapper encapsulates any unsecured network communication inside
secure SSL tunnel. Unsecured communication looks like this:

    CLIENT  <--------------------- plain TCP -------------------->  SERVER

Comminication that is secured by SSL Wrapper looks like this:

    CLIENT <-> WRAPPER_CLIENT <====== SSL =====> WRAPPER_SERVER <-> SERVER

    WRAPPER_CLIENT: ssl_wrapper -c port:wrapper_server_host:wrapper_server_port
	WRAPPER_SERVER: ssl_wrapper -s port:server_host:server_port

Instead of connecting directly to the SERVER, CLIENT is configured to connect
to the WRAPPER_CLIENT. WRAPPER_CLIENT encrypts the data and passes it over
to the WRAPPER_SERVER, and WRAPPER_SERVER passes it over to the SERVER.
Communication CLIENT <-> WRAPPER_CLIENT and WRAPPER_SERVER <-> SERVER remains
unencrypted.

What is the benefit in using SSL Wrapper? Both CLIENT and SERVER need no
modification to have their communication encrypted. For example, a SERVER
that does not support SSL, like a Web server, can be easily turned into a
server that does support SSL by putting WRAPPER_SERVER in front of it.
WRAPPER_SERVER is usually called 'SSL termination proxy'.

## Configuration options

    Usage: ssl_wrapper [OPTIONS]
    Available options:
      -p tcp_port:wrapper_host:wrapper_port
      -s ssl_port:server_host:server_port

## Examples

Enable SSL on a web server (listen on port 443, terminate SSL and forward
all traffic to port 80, which is a web port):

	ssl_wrapper -s 443:127.0.0.1:80

## Building SSL wrapper

	git clone https://github.com/cesanta/net_skeleton.git
	git clone https://github.com/cesanta/ssl_wrapper.git
	cd ssl_wrapper
	make

## License

SSL Wrapper is released under
[GNU GPL v.2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
Businesses have an option to get non-restrictive commercial
license and professional support from [Cesanta Software](http://cesanta.com).
