TARGET=anf2weight
SRC= lint.cpp anfweight.cpp main.cpp
CFLAGS= -O3


all:
	g++ $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f anf2weight

