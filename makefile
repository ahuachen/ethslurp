cflags=-Wall -O2 -DLINUX -I. -I./utillib

product=ethslurp

libs=utillib/utillib.a /usr/lib/libcurl.dylib

src= \
ethslurp.cpp \
manage.cpp \
transaction.cpp \
transaction_custom.cpp

#-------------------------------------------------
# probably don't have to change below this line

objects = $(patsubst %.cpp,objs/%.o,$(src))

all:
	cd utillib; make; cd ..
	@echo "$(product) build started"
	@echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@make $(product)
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

$(product): $(objects) $(libs)
	g++ -o $(product) $(objects) $(libs)
	@strip $(product)
	@mv $(product) /Users/jrush/source

$(objects): | objs

objs:
	@mkdir -p $@

objs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(cflags) -c $< -o $@

clean:
	@cd utillib; make clean; cd ..
	-@$(RM) -f $(product) $(objects) 2> /dev/null
