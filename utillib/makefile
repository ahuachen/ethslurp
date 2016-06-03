cflags=-Wall -O2 -DLINUX

product=utillib.a

src= \
attribute.cpp \
basenode.cpp \
config.cpp \
curl_code.cpp \
database.cpp \
dates.cpp \
editableobject.cpp \
editableobjectbase.cpp \
exportcontext.cpp \
importcontext.cpp \
fielddata.cpp \
fieldlist.cpp \
runtimeclass.cpp \
sfarchive.cpp \
sfdate.cpp \
sffile.cpp \
sfos.cpp \
sfstring.cpp \
sftime.cpp \
sftimeofday.cpp \
sftimespan.cpp \
version.cpp

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
