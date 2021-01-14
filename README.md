# memsim

skeleton code for Assignment 5 Question 1

# test files:
```
$ wc -l *.txt
      17 test1.txt
       1 test2.txt
       1 test3.txt
   68807 test4.txt
 1000000 test5.txt
 1000000 test6.txt
 2068826 total
```
---
# My results:
```
$ ./memsim 123 < test1.txt
pages requested:    57
largest free chunk: 110
elapsed time:       0.000

$ ./memsim 321 < test2.txt
pages requested:    16
largest free chunk: 136
elapsed time:       0.000

$ ./memsim 111 < test3.txt
pages requested:    0
largest free chunk: 0
elapsed time:       0.000

$ ./memsim 222 < test4.txt
pages requested:    896
largest free chunk: 1000
elapsed time:       0.006

$ ./memsim 333 < test5.txt
pages requested:    121597
largest free chunk: 9445200
elapsed time:       0.678

$ ./memsim 606 < test6.txt
pages requested:    2682392
largest free chunk: 9996
elapsed time:       1.277
```
