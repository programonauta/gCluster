#define the compiler

CXX=g++

#define compile-time flags

CXX_FLAGS = -std=c++11 -g

INCLUDES = -I./include -I./csv-reader -I./point -I./graphs

SRCS =  src/cell.cpp src/gcluster.cpp csv-reader/csv-reader.cpp point/point.cpp graphs/graphs.cpp

# define the executable file 
MAIN = gcluster

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

all:    $(MAIN)
	@echo  Simple compiler named mycc has been compiled

$(MAIN): $(OBJS) 
	$(CXX) $(CXX_FLAGS) $(SRCS) $(INCLUDES) -o $(MAIN) $(OBJS) 

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
