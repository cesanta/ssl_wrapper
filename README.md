ssl_wrapper
===========

SSL Wrapper encapsulates any unsecured network communication inside
secure SSL tunnel. Unsecured communication looks like this:

    CLIENT  <--------------------- plain TCP -------------------->  SERVER

Comminication that is secured by SSL Wrapper looks like this:

    CLIENT <-> WRAPPER_CLIENT <====== SSL =====> WRAPPER_SERVER <-> SERVER

    WRAPPER_CLIENT: ssl_tunnel -P port:wrapper_server_host:wrapper_server_port
	WRAPPER_SERVER: ssl_tunnel -S port:server_host:server_port

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

## Examples