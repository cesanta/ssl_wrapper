ssl_wrapper
===========

SSL Wrapper is an application that encrypts any unsecured network
communication inside secure SSL tunnel and provides industry-grade
authentication mechanism. To achieve that, only minor re-configuration
is required instead software update that costs time and money.

Graphics below illustrate how SSL Wrapper works:

<img src="http://cesanta.com/images/ssl_wrapper/img_1.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_2.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_3.png" style="width:80%" />
<img src="http://cesanta.com/images/ssl_wrapper/img_4.png" style="width:80%" />

## Configuration options

    Usage: ssl_wrapper [OPTIONS]
    Available options:
      -p tcp_port:wrapper_host:wrapper_port
      -s ssl_port:server_host:server_port

## Examples

Enable SSL on a web server (listen on port 443, terminate SSL and forward
all traffic to port 80, which is a web port):

	ssl_wrapper -s 443:127.0.0.1:80

<img src="http://cesanta.com/images/ssl_wrapper/img_3.png" style="width:80%" />


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
