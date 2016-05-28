cflags=-Wall -O2 -DLINUX -I. -I./utillib

product=ethslurp

libs=utillib/utillib.a /usr/lib/libcurl.dylib

src= \
ethslurp.cpp \
slurper.cpp \
manage.cpp \
options.cpp \
transaction.cpp \
transaction_custom.cpp \
slurp.cpp \
slurp_custom.cpp

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

$(objects): | objs

objs:
	@mkdir -p $@

objs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(cflags) -c $< -o $@

cleanall:
	clean

clean:
	@cd utillib; make clean; cd ..
	-@$(RM) -f $(product) $(objects) 2> /dev/null
