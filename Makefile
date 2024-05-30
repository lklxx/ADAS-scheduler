INC=-Isrc/lib -Isrc/include/ADASScheduler

.DEFAULT_GOAL := adas-schedule

adas-schedule: src/main.cc
	g++ -Wall $(INC) src/main.cc -o adas-scheduler

clean:
	rm -f adas-scheduler
