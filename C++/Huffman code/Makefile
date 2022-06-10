COMPILER=g++
CXXFLAGS=-Wall -pedantic
EXECUTABLE=decompressor

all: compile

compile: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): main.cpp
	$(COMPILER) $^ -o $@

clean:
	rm -f *.o *~ $(EXECUTABLE) tempfile
