ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTINCLUDE  := -I$(shell root-config --incdir)

SRCS = functions.cpp twopeg_merge.cpp

OBJS = $(SRCS:.cpp=.o)

all: twopeg_merge

%.o: %.cpp
	$(CXX) $(ROOTCFLAGS) $(ROOTINCLUDE) -c $< -o $@

twopeg_merge: $(OBJS)
	$(CXX) $(ROOTCFLAGS) $(ROOTINCLUDE) -o $@ $^ $(ROOTLIBS)


clean:
	rm functions.o twopeg_merge.o twopeg_merge
	