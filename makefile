cflags=-Wall -O2 -DLINUX -I. -I./utillib -I./etherlib

product=ethslurp

libs=etherlib/etherlib.a utillib/utillib.a /usr/lib/libcurl.dylib

src= \
ethslurp.cpp \
options.cpp

#-------------------------------------------------
# probably don't have to change below this line

objects = $(patsubst %.cpp,objs/%.o,$(src))

all:
	@cd utillib; make; cd ..
	@cd etherlib; make; cd ..
	@echo "$(product) build started"
	@echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@make $(product)
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

hack: all
	@cd theHack; make; cd ..
	@make -B back

test: all
	@cd theData/tests; make; cd ..
	@cd theData/data; make; cd ..
	@cd theData/funcs; make; cd ..
	@cd theData/ghc; make; cd ..
	@cd theData/otherContracts; make; cd ..
	@cd theData/whales; make; cd ..
	@make -B back

back:
	@rm -fR theData/backup
	@cp -pR ~/.ethslurp theData/backup
	@rm -fR theData/backup/slurps

$(product): $(objects) $(libs)
	g++ -o $(product) $(objects) $(libs)
	@strip $(product)

$(objects): | objs

objs:
	@mkdir -p $@

objs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(cflags) -c $< -o $@

cleanall:
	@make clean

clean:
	@cd utillib; make clean; cd ..
	@cd etherlib; make clean; cd ..
	-@$(RM) -f $(product) $(objects) 2> /dev/null
