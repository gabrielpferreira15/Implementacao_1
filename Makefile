CC = gcc
CFLAGS = -Wall -Wextra -std=c17

TARGET = processflow
OBJ = main.o task.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c task.h
	$(CC) $(CFLAGS) -c main.c

task.o: task.c task.h
	$(CC) $(CFLAGS) -c task.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: run clean