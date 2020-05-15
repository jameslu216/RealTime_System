all:
	g++  -o simple_processes simple_processes.c
	g++ -o simple_threads simple_threads.c -pthread
	g++ -o q1 q1.cpp -pthread
	g++ -o pivp priority_inversion.cpp -pthread
	g++ -o pcp priority_inversion.cpp -pthread
	g++ -o pihp priority_inheritance.cpp -pthread
clean:
	rm simple_processes
	rm simple_threads
	rm q1
	rm pivp
	rm pcp
	rm pihp
