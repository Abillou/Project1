all: clean singleProcess bfs bfsSignal dfs
	echo "All files compiled"

singleProcess: singleProcess.c
	gcc -o singleProcess singleProcess.c

bfs: bfs.c
	gcc -o bfs bfs.c -lm

bfsSignal: bfsSignal.c
	gcc -o bfsSignal bfsSignal.c -lm

dfs: dfs.c
	gcc -o dfs dfs.c

clean:
	rm -f singleProcess *.o bfs bfsSignal dfs

pack:
	tar -cvf Project1.tar *.c *.h Makefile README.txt