TARGET= single_cycle

$(TARGET): single_cycle.o CPU.o Memory.o
	g++ -o $(TARGET) single_cycle.o CPU.o Memory.o -DDEBUG

single_cycle.o: single_cycle.cpp
	g++ -c single_cycle.cpp
	
CPU.o: CPU.cpp
	g++ -c CPU.cpp

Memory.o: Memory.cpp
	g++ -c Memory.cpp

clean:
	rm -f *.o $(TARGET)
