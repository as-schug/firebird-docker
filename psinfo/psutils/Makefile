CFLAGS=-g -Wall -D__LINUX__ -fPIC
CC=gcc
AR=ar

PSLIBDIR=/usr/lib
PSLIBFILE=libps_utils.so

all: $(PSLIBFILE) 

install: $(PSLIBFILE)
	cp $(PSLIBFILE) $(PSLIBDIR)
	mkdir -p /usr/include/psinfo/	
	cp ps_utils.h /usr/include/psinfo/
	@echo $(PSLIBFILE) esta em $(PSLIBDIR) e ps_utils.h em /usr/include/psinfo
	
$(PSLIBFILE): ps_utils.o ps_utils.h
	rm -f $(PSLIBFILE)
	$(CC) -lssl -shared -o $(PSLIBFILE) ps_utils.o

clean:
	rm -f *.o $(PSLIBFILE)
	

debug:
	$(CC) -DPSDEBUG -lssl -o ps_utils ps_utils.c
	rm -f ps_utils.o
