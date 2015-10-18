#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Simulation/SimBuilder.h"

using namespace SAPHRON;

int main(int argc, char const* argv[])
{
	std::cout << "                                                                         \n" << 
	             " ******************************************************************      \n" << 
	             " *       ____      _     ____   _   _  ____    ___   _   _        *      \n" << 
	             " *      / ___|    / \\   |  _ \\ | | | ||  _ \\  / _ \\ | \\ | |       * \n" << 
	             " *      \\___ \\   / _ \\  | |_) || |_| || |_) || | | ||  \\| |       *  \n" << 
	             " *       ___) | / ___ \\ |  __/ |  _  ||  _ < | |_| || |\\  |       *    \n" << 
	             " *      |____/ /_/   \\_\\|_|    |_| |_||_| \\_\\ \\___/ |_| \\_|       *\n" << 
	             " *                                                                *      \n" << 
	             " * \033[1mS\033[0mtatistical \033[1mA\033[0mpplied \033[1mPH\033[0mysics "   <<
	             "through \033[1mR\033[0mandom \033[1mO\033[0mn-the-fly \033[1mN\033[0mumerics *\n" << 
	             " ******************************************************************      \n" << 
	             "                                                                         \n";

	SimBuilder builder;
	builder.BuildSimulation(std::cin);

	return 0;
}	