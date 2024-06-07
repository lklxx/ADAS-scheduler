# ADAS-scheduler

C++ implementation of the paper [Mapping and Scheduling Automotive Applications on ADAS Platforms using Metaheuristics](https://ieeexplore.ieee.org/document/9212029).

To run the scheduler, first compile the program:
```
$ make
```

Run the scheduler:
```
$ ./adas-scheduler --input=string [options] ...
```

with the below arguments provided:
```
  -i, --input     input data with test case (string)
  -o, --output    output file for scheduling result (string [=])
  -t, --time      execution time for simulated annealing (int [=15])
  -?, --help      print this message
```

For example, to run the scheduler with the input data:
```
$ ./adas-scheduler -i test/input/input1.txt 
```
