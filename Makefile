app: main.o
	gcc main.o -o processflow

main.o: main.c
	gcc -c main.c

run: app
	./app

clean:
	rm -f *.o processflow

.PHONY: run clean