CXX=g++
CXXFLAGS=-O3
SYSFLAGS=-std=gnu++11
TARGET=test.cpp
ALL=origin tiled invert invert-tiled-in invert-tiled-out
OPT=origin-opt tiled-opt invert-opt invert-tiled-in-opt invert-tiled-out-opt

.PHONY: all opt clean clear-log, perf

all: $(ALL)

origin: $(TARGET)
	$(CXX) $(SYSFLAGS) -DORIGIN $^ -o $@

tiled: $(TARGET)
	$(CXX) $(SYSFLAGS) -DTILED $^ -o $@

invert: $(TARGET)
	$(CXX) $(SYSFLAGS) -DINVERT $^ -o $@

invert-tiled-in: $(TARGET)
	$(CXX) $(SYSFLAGS) -DINV_TILED_INNER $^ -o $@

invert-tiled-out: $(TARGET)
	$(CXX) $(SYSFLAGS) -DINV_TILED_OUTER $^ -o $@


opt: $(OPT)

origin-opt: $(TARGET)
	$(CXX) $(SYSFLAGS) $(CXXFLAGS) -DORIGIN $^ -o $@

tiled-opt: $(TARGET)
	$(CXX) $(SYSFLAGS) $(CXXFLAGS) -DTILED $^ -o $@

invert-opt: $(TARGET)
	$(CXX) $(SYSFLAGS) $(CXXFLAGS) -DINVERT $^ -o $@

invert-tiled-in-opt: $(TARGET)
	$(CXX) $(SYSFLAGS) $(CXXFLAGS) -DINV_TILED_INNER $^ -o $@

invert-tiled-out-opt: $(TARGET)
	$(CXX) $(SYSFLAGS) $(CXXFLAGS) -DINV_TILED_OUTER $^ -o $@

clear-log:
	rm $(ALL:=.log) $(OPT:=.log)

debug:
	$(CXX) -g $^ -o $@

perf:
	perf stat -B -e cache-references,cache-misses,LLC-load-misses,LLC-load,LLC-store-misses,LLC-store,cycles,instructions,branches,faults,migrations,LLC-prefetches,LLC-prefetch-misses ${target}

clean:
	rm $(ALL) $(OPT)
