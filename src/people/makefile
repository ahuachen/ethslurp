cflags=-I../utillib -Wall -O2 -DLINUX

product=peoplelib.a

src= \
editableobject.cpp \
emailsettings.cpp \
loginsettings.cpp \
peopledatabase.cpp \
peopledatabase_group.cpp \
peopledatabase_header.cpp \
peopledatabase_mail.cpp \
peopledatabase_ob.cpp \
peopledatabase_perm.cpp \
peopledatabase_reg.cpp \
peopledatabase_submission.cpp \
peopledatabase_user.cpp \
peopledatabase_usersettings.cpp \
person_4_0_1.cpp \
person.cpp \
protectedobject.cpp \
registration.cpp \
simpleforum.cpp \
simplelogin.cpp \
simpleloginreset.cpp \
simplemenu.cpp \
simplemyaccount.cpp \
simplenewaccount.cpp \
simplepage.cpp \
simplerecover.cpp \
simpleserver.cpp \
simplethread.cpp \
simplethreads.cpp \
simpleuserdb.cpp \
subsettings.cpp \
usergroup_4_0_1.cpp \
usergroup.cpp \
usersettings.cpp

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
#	@chown calendar.calendar $(product)

$(objects): | objs

objs:
	@mkdir -p $@

objs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(cflags) -c $< -o $@

cleanall:
	@cd lib_util ; make cleanall
	@make clean

clean:
	-@$(RM) -f $(product) $(objects) 2> /dev/null
