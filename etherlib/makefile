cflags=-Wall -O2 -DLINUX -I../utillib -I../ -I./ -I../../ethslurp

product=etherlib.a

src= \
account.cpp \
account_custom.cpp \
function.cpp \
function_custom.cpp \
parameter.cpp \
parameter_custom.cpp \
proposal.cpp \
proposal_custom.cpp \
slurp.cpp \
slurp_custom.cpp \
transaction.cpp \
transaction_custom.cpp \
vote.cpp \
vote_custom.cpp \
webapi.cpp

#-------------------------------------------------
# probably don't have to change below this line

objects = $(patsubst %.cpp,objs/%.o,$(src))

all:
	@echo "$(product) build started"
	@echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
	@make $(product)
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

$(product): $(objects)
	-@$(RM) -f $*.a
	@ar qc $*.a $(objects)
#	@chown jrush.jrush $(product)

$(objects): | objs

objs:
	@mkdir -p $@

objs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(cflags) -c $< -o $@

clean:
	-@$(RM) -f $(product) $(objects) 2> /dev/null
