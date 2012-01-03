OBJS = tsp.o render.o
CXXFLAGS = `sdl-config --cflags` -g -pg
LDFLAGS = `sdl-config --libs`  -lGL

all: tsp

tsp: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@


%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.d tsp
