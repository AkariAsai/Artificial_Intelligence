CC=gcc
OBJS=main.o gamerule.o montecarlo.o minimax.o
CFLAGS=-Wall -O3
TARGET=main

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc -o $@ $(OBJS) -lm

clean:
	rm $(TARGET) *.o
