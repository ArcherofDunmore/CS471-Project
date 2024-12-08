CS471 Project
# CS471 Operating Systems Project Fall 2024

Execution of this program:
 - Open a terminal, navigate to the folder `Part2-Synchronize`.
 - Run the program by entering `.\synchronizer.exe [sleep_time (sec)]`.

The program will run a thread synchronization algorithm, using multiple different parameters. It will test the algorithm on multiple numbers of producers and consumers, and the sleep time entered by the user for all of the producer/consumer iterations.

> At the beginning of their function loop, the producer and consumer functions sleep for a random time between 0 and 15 seconds.
> The cap of 15 seconds was given to keep the program execution time lower.

As the program runs through each test it will display the number being inserted or removed, as well as the current state of the buffer.

Many of the numbers being inserted into the buffer are the same. This is because the C++ function rand() is not truly random, and will produce the same sequence in the same order if given the same seed. Because all of the threads are branching from the synchronizer function, all instances of the producer function will have the same sequence of "random" values in the same order.

Once the program finishes executing all of the test cases, it will print all of the collected turnaround times, and averages, to two files: one called "producer.txt", and one called "consumer.txt". These files will have the following format:

```
test [1]: num_producers = [a1], num_consumers = [b1], sleep_time = [c1]
	all turnaround times: [ X.XXXXXXX X.XXXXXXX ]
	average turaround time: X.XXXXXXX

...

test [n]: num_producers = [an], num_consumers = [bn], sleep_time = [cn]
	all turnaround times: [ X.XXXXXXX X.XXXXXXX ]
	average turaround time: X.XXXXXXX

overall average turnaround time: X.XXXX
```

From running the tests with a few different sleep times, a few patterns become obvious. When a test is run with a higher number of one thread than another, for example, test 3 in the example results below, where there are 16 producer threads and only 1 consumer thread, the larger count threads often have a much higher average turnover time. In the example below, this is because the producers are so large in number they immediately fill up the buffer and then must wait for the single consumer to consume an item, which will almost immediately get refilled by another producer. Conversely, the side with the smaller number (in this case, the consumer) has a much smaller average turnaround time, 0.004 seconds to the producers 1.005 seconds, because anytime the consumer wants to consume there is likely to be many items already in the buffer.

The test results for the program run with a sleep_time of 10 seconds are as follows:

`producer.txt`
```
test 1: num_producers = 1, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0011159 0.0010973 ]
	average turaround time: 0.0011066

test 2: num_producers = 4, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0013918 0.002632 0.0038778 0.0047456 0.0020774 0.0034589 3.00323 ]
	average turaround time: 0.43163

test 3: num_producers = 16, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0024397 0.0035071 0.004821 0.0045977 0.0058239 0.0073031 7.01046 ]
	average turaround time: 1.00556

test 4: num_producers = 1, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.0017135 0.0019959 ]
	average turaround time: 0.0018547

test 5: num_producers = 4, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.0019707 0.0027649 0.0038131 0.0047392 0.0019568 0.0030569 0.0048763 3.01822 ]
	average turaround time: 0.380175

test 6: num_producers = 16, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.0021829 0.0034039 0.0044318 0.0054644 0.0063815 0.0087883 0.0095266 7.01147 7.02897 ]
	average turaround time: 1.56451

test 7: num_producers = 1, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0024358 0.0019302 ]
	average turaround time: 0.002183

test 8: num_producers = 4, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0018879 0.0032661 0.0048988 0.006527 0.0017543 0.0031164 0.0041785 0.0053104 ]
	average turaround time: 0.00386742

test 9: num_producers = 16, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0020547 0.0031113 0.0043058 0.0054928 0.0065158 0.009413 0.0101621 0.0109466 0.0117744 7.02148 7.02244 7.02327 7.02413 ]
	average turaround time: 2.16578

test 10: num_producers = 1, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0015928 0.0014583 ]
	average turaround time: 0.00152555

test 11: num_producers = 4, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0025899 0.0036848 0.0047758 0.0060186 0.0009802 0.0022239 0.0031503 0.0041782 ]
	average turaround time: 0.00345021

test 12: num_producers = 16, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0018948 0.0030717 0.0042246 0.0051077 0.0060495 0.0096304 0.0103346 0.0109496 0.0113939 0.0120739 0.0150066 0.0154368 0.0160564 0.0169075 0.0176693 0.0205527 0.0016272 0.0028377 0.0040193 0.0047311 0.0056692 3.01075 3.01167 3.01234 3.01309 3.01381 3.0085 3.00916 3.00984 3.0106 3.01133 3.01426 ]
	average turaround time: 1.04127

overall average turnaround time: 0.8873
```

`consumer.txt`
```
test 1: num_producers = 1, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0015743 0.0011949 ]
	average turaround time: 0.0013846

test 2: num_producers = 4, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0055298 0.0024738 ]
	average turaround time: 0.0040018

test 3: num_producers = 16, num_consumers = 1, sleep_time = 10
	all turnaround times: [ 0.0065297 0.0019467 ]
	average turaround time: 0.0042382

test 4: num_producers = 1, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.0025313 4.01772 ]
	average turaround time: 2.01012

test 5: num_producers = 4, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.005384 0.0060168 0.0024016 0.003338 ]
	average turaround time: 0.0042851

test 6: num_producers = 16, num_consumers = 2, sleep_time = 10
	all turnaround times: [ 0.0075995 0.0082379 0.0017824 0.002663 ]
	average turaround time: 0.0050707

test 7: num_producers = 1, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0036206 4.01928 ]
	average turaround time: 2.01145

test 8: num_producers = 4, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0073678 0.0081194 0.0089226 0.0095217 0.0019223 0.0030706 0.0039666 0.0046922 ]
	average turaround time: 0.0059479

test 9: num_producers = 16, num_consumers = 4, sleep_time = 10
	all turnaround times: [ 0.0073114 0.0078124 0.0083607 0.0089069 0.0023337 0.0034394 0.0045899 0.0057745 ]
	average turaround time: 0.00606611

test 10: num_producers = 1, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0024486 4.01749 ]
	average turaround time: 2.00997

test 11: num_producers = 4, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0069991 0.0076813 0.0081293 0.0085424 4.01933 4.02052 4.02118 4.02155 ]
	average turaround time: 2.01424

test 12: num_producers = 16, num_consumers = 16, sleep_time = 10
	all turnaround times: [ 0.0067626 0.0077167 0.0082632 0.0087702 0.0091902 0.0126454 0.0134269 0.0138883 0.0143145 0.014653 0.0181603 0.0187537 0.0191707 0.0195783 0.0200192 0.0210846 0.0018691 0.0029986 0.0040238 0.0046014 0.0049905 0.0097052 0.0102832 0.010757 0.0111127 0.0114113 0.0080255 0.0085053 0.0091318 0.0096402 0.0099312 0.0106818 ]
	average turaround time: 0.0110646

overall average turnaround time: 0.377419
```