#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Worlds/World.h"
#include "Simulation/SimException.h"

using namespace SAPHRON;

int DumpErrorsToConsole(std::vector<std::string> msgs, int notw)
{
	std::cout << std::setw(notw) << std::right << "\033[1;31mError(s)! See below.\033[0m\n";
	for(auto& msg : msgs)
			std::cout << "   * " << msg << "\n";
	return -1;
}

void DumpNoticesToConsole(std::vector<std::string> msgs, std::string prefix, int notw)
{
	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";
	if(msgs.size() == 0)
		return;
	
	for(auto& msg : msgs)
		std::cout << prefix << " * " << msg << "\n";
}

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

	int ltot = 77;
	int msgw = 47;
	int notw = ltot - msgw;

	World* world = nullptr;
	try{
		std::cout << std::setw(msgw) << std::left << " > Validating JSON...";
		world = World::BuildWorld(std::cin);
		DumpNoticesToConsole({}, "", notw);
	} catch(BuildException& e)
	{
		DumpErrorsToConsole(e.GetErrors(), notw);
		return -1;
	}

	delete world;

	return 0;
}	