#include "simulator.hpp"

int main()
{
	Simulator* simulator = new Simulator();
	int code = simulator->start();
	if(code == SUCCESS) puts("\n---- Simulatation Stops ! -----\n");
	return 0;
}
