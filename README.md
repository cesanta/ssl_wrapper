ssl_wrapper
===========

SSL Wrapper is an application that encrypts any unsecured network
communication inside secure SSL tunnel and provides industry-grade
authentication mechanism. 

SSL wrapper opens a listening port and forwards traffic to the specified target host, in two modes:

   1.  Listening port is SSL, target host is plain TCP
   2.  Listening port is plain TCP, target host is SSL

SSL wrapper can be used just on one side of communication, or on both sides:

- on one side (client), SSL wrapper encrypts outgoing client traffic
- on one side (server), SSL wrapper decrypts incoming server traffic,
  implementing
  [SSL termination proxy](http://en.wikipedia.org/wiki/SSL_termination_proxy)
- on both sides, two paired SSL wrappers enclose plain TCP traffic
  into the secure SSL tunnel

Graphics below illustrate how SSL Wrapper works:

<img src="http://cesanta.com/images/ssl_wrapper/img_1.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_2.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_3.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_4.png" style="width:80%" />

## Configuration options

    Usage: ssl_wrapper <listening_address> <target_address>

Address format is as follows:

    [PROTO://][IP:]PORT[:SSL_CERT.PEM][:CA_CERT.PEM]

PROTO is either `tcp` or `ssl`. If omitted, `tcp` is used. For SSL,
`SSL_CERT.PEM` specifies server certificate for `listening_address` or
client certificate for `target_address`. All certificates must be in PEM format.
Server certificate `SSL_CERT.PEM` file must have both server certificate and
private key, in PEM format, concatenated together.

If `CA_CERT.PEM` is specified, it enables peer certificate authentication.
For `listening_address` it enables client side certificate auth, also
known as two-way SSL.


## Examples

Enable SSL on a web server (listen on port 443, terminate SSL and forward
all traffic to port 80, which is a web port):

	   ssl_wrapper ssl://443:server_cert.pem 127.0.0.1:80

<img src="http://cesanta.com/images/ssl_wrapper/img_3.png" style="width:80%" />

Setup SSL man-in-the-middle (MITM) proxy for the external site foo.com,
which requires client authentication. By setting up such a proxy, we can
connect to foo.com without client-side certificate. Only proxy needs to have
client-side certificate:

    ssl_wrapper ssl://443:foo_server.pem ssl://foo.com:443:foo_client.pem


## Building SSL wrapper

	git clone https://github.com/cesanta/net_skeleton.git
	git clone https://github.com/cesanta/ssl_wrapper.git
	cd ssl_wrapper
	make

Note: SSL Wrapper is built on top of
[Net Skeleton](https://github.com/cesanta/net_skeleton), and therefore
it is portable and works on may operating systems including Windows, MacOS, UNIX/LINUX, QNX, eCos, Android, iOS, and more.

## License

SSL Wrapper is released under
[GNU GPL v.2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
Businesses have an option to get non-restrictive commercial
license and professional support from [Cesanta Software](http://cesanta.com).
