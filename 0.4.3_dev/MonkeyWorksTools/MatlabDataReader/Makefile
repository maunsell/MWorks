MEXSUFFIX  = mexmaci
#MATLABHOME = /Applications/MATLAB_R2008b
# This symbolic link needs to exist for the matlab window anyway
MATLABHOME=/Library/MonkeyWorks/Matlab/MATLAB
MW_HOME = /Library/MonkeyWorks
MEX        = $(MATLABHOME)/bin/mex
CXX        = g++
CFLAGS     = -fno-common -no-cpp-precomp -fexceptions -g
LIBS      = -lscarab -ldfindex -L$(MW_HOME)/lib -lMonkeyWorksStreamUtilities -lboost_serialization-mt -lboost_system-mt -lboost_filesystem-mt
INCLUDE   = -I$(MATLABHOME)/extern/include -I$(MW_HOME)/include 
MEXFLAGS  = -cxx -g CC='$(CXX)' CXX='$(CXX)' LD='$(CXX)'

all: getEvents.$(MEXSUFFIX) getCodecs.$(MEXSUFFIX)
	mkdir -p /Library/MonkeyWorks/Matlab;
	cp getEvents.$(MEXSUFFIX) /Library/MonkeyWorks/Matlab/getEvents.$(MEXSUFFIX);
	cp getCodecs.$(MEXSUFFIX) /Library/MonkeyWorks/Matlab/getCodecs.$(MEXSUFFIX);

clean:
	rm -f *.o \
	*.$(MEXSUFFIX) \
	/Library/MonkeyWorks/Matlab/*.$(MEXSUFFIX)

getEvents.$(MEXSUFFIX): getEvents.o
	$(MEX) $(MEXFLAGS) $(LIBS) -output getEvents $^

getEvents.o: getEvents.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $^

getCodecs.$(MEXSUFFIX): getCodecs.o
	$(MEX) $(MEXFLAGS) $(LIBS) -output getCodecs $^

getCodecs.o: getCodecs.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $^

