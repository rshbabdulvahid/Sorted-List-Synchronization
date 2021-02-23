NAME: Rishab Abdulvahid
EMAIL: rabdulvahid23@g.ucla.edu

Meaningful contents:
	-Makefile
	-lab2_list.c
	-data_producer.sh
	-lab2b.gp

This code was created to demonstrate serialized operations on a circular sorted linked list, showing how to parallelize
the shared usage of this resource. The code supports a [--lists] option, which determines how many sub-lists the overall
linked list will be partitioned into. Each thread, based on the key it's about to insert into the list (modulo'd by the
number of total sub-lists), will find a sub-list to perform operations on. The thread will insert some number of elements
into the list, look them up afterwards, and delete the inserted elements. The total length of the linked list will be
checked after inserts and after deletes to make sure there are no errors with serialization (if there are, the code will
report them and stop). The code also supports [--yield] options, which will force a thread to yield during a linked list
operation: this is used to maximize contention and cause race conditions when they need to be demonstrated. The code supports
serialization using either mutex or spin-lock synchronization. Ultimately, the code will produce a comma-separated summary
of its operations on the linked list. Using a graph script, operating on a list of CSV output from the code, 5 graphs can
be produced.

Output: name of test, number of threads, number of iterations, number of lists, total operations performed, run time in ns,
average time/op, and average wait-for-lock (for mutex operations)

Graphs:
	lab2b_1: Throughput versus number of threads for mutex and spin-lock synchronized operations on a single list.
		 Demonstrates how parallelizing a shared resource does not scale well with multiple threads (unless the
		 resource is partitioned in some way).
	lab2b_2: Wait-for-lock time and time/op for a single list, synchronized with mutex locking, against the number
		 of threads. Demonstrates that many threads spend significant amount of time waiting for the lock, which
		 is not efficient parallelism.
	lab2b_3: Demonstrates correctness of synchronization by indicating successful runs over various number of threads
		 on 4 sub-lists.
	lab2b_4: Throughput versus number of threads for various numbers of sub-lists protected w/ mutex. Demonstrates
		 how increasing sub-lists generates an increase in throughput for various numbers of threads.
	lab2b_5: Throughput versus number of threads for various numbers of sub-lists protected w/ spin-lock. Demonstrates
		 how increasing sub-lists generates an increase in throughput for various numbers of threads.

Usage: ./lab2_list [--threads=THREADNUM] [--iterations=NUM] [--lists=NUMLISTS] [--yield=idl] [--sync=m,s]
       		-threads designate the number of threads to be used
		-iterations designates how many elements will be added to the list by each thread
		-lists designates how many sub-lists the overall list will be partitioned into
		-yield designates where the forced yields, designed to maximize conflict and contention, will be activated:
		       -i indicates a yield in the insert operation
		       -l indicates a yield in the lookup operation
		       -d indicated a yield in the delete operation
		 Any combination of yield operations are supported
		-sync designates what type of synchronization will be used
		       -s indicates spin-locking
		       -m indicated mutex-locking
		 Only one of the two should be passed

Makefile included supports 4 useful targets:
	 -build builds the executable lab2_list
	 -tests creates a CSV file from data_producer.sh, used to produce the 5 graphs
	 -graphs produces the graphs from the CSV, using the graphing script lab2b.gp
	 -clean will remove the executable	 
	 


![Screenshot](/lab2b_1.png) 

![Screenshot](/lab2b_2.png) 

![Screenshot](/lab2b_3.png) 

![Screenshot](/lab2b_4.png) 

![Screenshot](/lab2b_5.png)
