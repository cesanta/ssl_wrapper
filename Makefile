NS = ../net_skeleton
DEFS = -I$(NS) -DNS_ENABLE_SSL $(CFLAGS_EXTRA)

ssl_wrapper:
	$(CC) ssl_wrapper.c $(NS)/net_skeleton.c -o $@ -W -Wall -O2 $(DEFS) -lssl

ssl_wrapper.exe:
	wine cl ssl_wrapper.c $(NS)/net_skeleton.c /I$(NS) /MD

clean:
	rm -rf ssl_wrapper ssl_wrapper.exe *.o *.obj *.dSYM
