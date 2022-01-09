# Final Project Makefile

# declare variable
CC=g++
SOURCES=main.cpp
OBJECTS=$(SOURCES:.c=.o)
BINDIR=../bin/
#BINDIR=./

EXE=project


all: $(SOURCES) $(EXE)
$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -std=c++14 -O3 -lm -o $(BINDIR)$@

clean:
	rm -rf *.o $(BINDIR)$(EXE)

