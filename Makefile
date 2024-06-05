INC=-Isrc/lib -Isrc/include/ADASScheduler -Isrc/include/SecondaryScheduler -Isrc/include/Schedule -Isrc/include/SimulatedAnnealing
CPPFLAGS=-Wall --std=c++20

.DEFAULT_GOAL := adas-schedule

adas-schedule: src/main.cc
	g++ $(CPPFLAGS) $(INC) src/main.cc -o adas-scheduler

clean:
	rm -f adas-scheduler
