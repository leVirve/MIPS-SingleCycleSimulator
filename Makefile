
TARGET=single_cycle

$(TARGET): single_cycle.o
	g++ single_cycle.o -o $(TARGET)

single_cycle.o: single_cycle.cpp
	g++ -c single_cycle.cpp

clean:
	rm -f *.o $(TARGET)