build: lab2_list.c SortedList.h SortedList.c
	gcc -g -Wall -Wextra -pthread -lprofiler SortedList.c lab2_list.c -o lab2_list

tests: data_producer.sh
	./data_producer.sh > lab2b_list.csv

graphs: lab2b.gp
	./lab2b.gp

clean:
	rm -f lab2_list
