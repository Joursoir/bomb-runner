CPPC = g++
CPPFLAGS = -Wall -g -lncurses
SOURCES = src/main.cpp src/Character.cpp
OBJECTS = main.o Character.o
EXECUTABLE = brun

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CPPC) $(CPPFLAGS) -o $(EXECUTABLE) $(OBJECTS)

$(OBJECTS):
	@$(CPPC) -c $(CPPFLAGS) $(SOURCES)

clean:
	@rm -rf $(EXECUTABLE) $(OBJECTS)