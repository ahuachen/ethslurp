OBJS = \
ethslurp.o \
manage.o

CPPFLAGS=-I.
CFLAGS=-Wall -O2 -DLINUX

SHIP=/Users/jrush/source
SHIP_EXE=ethslurp
GOLD=/Users/jrush/source

LIBS= utillib/utillib.a /usr/lib/libcurl.dylib

all:
	cd utillib; make; cd ..
	make ship

ship: ethslurp
	strip ethslurp
	cp -p ethslurp $(SHIP)/$(SHIP_EXE)

ethslurp: $(OBJS) $(LIBS)
	g++ -o ethslurp $(OBJS) $(LIBS)

.cpp.o:
	g++ $(CPPFLAGS) $(CFLAGS) -c $<

cleanall:
	cd utillib; make clean; cd ..
	make clean

clean:
	rm -f $(OBJS)
	rm -f *.cgi
	rm -f *~
