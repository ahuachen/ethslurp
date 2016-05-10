cflags=-Wall -O2 -DLINUX

product=utillib.a

src= \
aes_encrypt.cpp \
attribute.cpp \
basenode.cpp \
cgilib.cpp \
colors.cpp \
colortable.cpp \
config.cpp \
curl_code.cpp \
database.cpp \
dates.cpp \
dateselect.cpp \
editableobjectbase.cpp \
element.cpp \
exportcontext.cpp \
fielddata.cpp \
fieldlist.cpp \
html.cpp \
importcontext.cpp \
jpg.cpp \
list.cpp \
mail.cpp \
parser.cpp \
paths.cpp \
permission.cpp \
point.cpp \
rect.cpp \
runtimeclass.cpp \
sfarchive.cpp \
sfdate.cpp \
sffile.cpp \
sffolder.cpp \
sfos.cpp \
sfstring.cpp \
sftime.cpp \
sftimeofday.cpp \
sftimespan.cpp \
string_table.cpp \
units.cpp \
version.cpp \
visitor.cpp

#matrix.cpp \
#vector.cpp \

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

cleanall:
	@cd lib_people ; make cleanall
	@make clean

clean:
	-@$(RM) -f $(product) $(objects) 2> /dev/null
