NS = ../net_skeleton
DEFS = -I$(NS) -DNS_ENABLE_SSL $(CFLAGS_EXTRA)
CFLAGS = -W -Wall -O2 -pthread -lssl $(DEFS)

ifeq ($(OS),Windows_NT)
 	# Windows build. Assumes that Visual Studio is installed at $(VC)
	VC = ../vc6
  OSSL = ../openssl-0.9.8
	CC = $(VC)/bin/cl
	CFLAGS = /MD /TC /nologo /W3 /I$(VC)/include /I$(OSSL)/include $(DEFS)
	CFLAGS += /link /incremental:no /libpath:$(VC)/lib /machine:IX86 $(OSSL)/lib/ssleay32.lib
endif

ssl_wrapper: ssl_wrapper.c $(NS)/net_skeleton.c
	$(CC) ssl_wrapper.c $(NS)/net_skeleton.c $(CFLAGS)

clean:
	rm -rf ssl_wrapper ssl_wrapper.exe *.o *.obj *.dSYM
