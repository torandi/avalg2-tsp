#LDFLAGS=
CXXFLAGS=-g -pg

OBJS = tsp.o# render.o
CFLAGS += -Wall -g
#LDFLAGS += -lSDL -lGL -lGLU

all: tsp 
 
tsp: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -rf *.o *.d tsp

%.o : %.cpp %.h
	@$(CXX) -MM $(CFLAGS) $< > $*.d
	$(CXX) $(CFLAGS) -c $< -o $@

