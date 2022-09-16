all:
	gcc stack.c eval.c main.c -o wonkytest
clean:
	rm -rf *.o wonkytest

