#
# Sin-Yaw Wang <swang24@scu.edu>
# BMP utility
#
STD=-std=c++11
OPT=-g
CXXFLAGS=${STD} ${OPT}

MKSRCS=makebmp.cxx
LIBSRCS=bmp.cxx genNewFname.cxx
EMBSRCS = embeddedbmp.cxx genNewFname.cxx bmp.cxx 
EXSRCS = extracttext.cxx bmp.cxx
MKOBJS=$(MKSRCS:.cxx=.o)
LIBOBJS=$(LIBSRCS:.cxx=.o)
EMBOBJS=$(EMBSRCS:.cxx=.o)
EXOBJS=$(EXSRCS:.cxx=.o)
ALLOBJS=$(MKOBJS) $(LIBOBJS) $(EMBOBJS) $(EXOBJS)
ALL=makebmp embed extract

%.o: %.cxx
	$(CXX) -c $(CXXFLAGS) $<

all:	$(ALL)

makebmp:	$(MKOBJS) $(LIBOBJS)
	$(CXX) -o $@ $+

embed:		$(EMBOBJS)
	$(CXX) -o $@ $+

extract:	$(EXOBJS)
	$(CXX) -o $@ $+

clean:
	/bin/rm -f $(ALLOBJS) $(ALL) test.bmp embedded_test.bmp
	/bin/rm -rf $(ALL:=.dSYM)

depend: $(LIBSRCS) $(MKSRCS) $(EMBSRCS) $(EXSRCS)
	TMP=`mktemp -p .`; export TMP; \
	sed -e '/^# DEPENDENTS/,$$d' Makefile > $$TMP; \
	echo '# DEPENDENTS' >> $$TMP; \
	$(CXX) -MM $+ >> $$TMP; \
	/bin/mv -f $$TMP Makefile

# DEPENDENTS
