# Copyright (c) 2014 Cesanta Software Limited
# All rights reserved
#
# To build SSL wrapper, first clone net_skeleton, then SSL wrapper:
# git clone https://github.com/cesanta/net_skeleton.git
# git clone https://github.com/cesanta/ssl_wrapper.git
# cd ssl_wrapper && make

NS = ../net_skeleton
DEFS = -I$(NS) -DNS_ENABLE_SSL $(CFLAGS_EXTRA)

ssl_wrapper:
	$(CC) ssl_wrapper.c $(NS)/net_skeleton.c -o $@ -W -Wall -O2 $(DEFS) -lssl

ssl_wrapper.exe:
	wine cl ssl_wrapper.c $(NS)/net_skeleton.c /I$(NS) /MD

clean:
	rm -rf ssl_wrapper ssl_wrapper.exe *.o *.obj *.dSYM
