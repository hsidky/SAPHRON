// This example is a demonstration of Semi-grand Density of States Sampling
// of a Lebwhol-Lasher binary mixture

// Include header files
// Include for parsing using stringstream
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Worlds/World.h"
#include "Worlds/SimpleWorld.h"
#include "Particles/Site.h"
#include "Simulation/SimBuilder.h"


using namespace SAPHRON;

int DumpErrorsToConsole(std::vector<std::string> msgs)
{
	for(auto& msg : msgs)
			std::cout << "   * " << msg << "\n";
	return -1;
}

void DumpNoticesToConsole(std::vector<std::string> msgs, std::string prefix)
{
	if(msgs.size() == 0)
		return;
	
	for(auto& msg : msgs)
		std::cout << prefix << " * " << msg << "\n";
}

// The main program expects a user to input the lattice size, number of EXEDOS
// iterations, minimum and maximum mole fractions, number of bins for density-of-states
// histogram and model, sites and vector file outputs.
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

	// Validate JSON.	
	std::cout << std::setw(msgw) << std::left << " > Validating JSON...";
	SimBuilder builder; 
	if(!builder.ParseInput(std::cin))
	{
		std::cout << std::setw(notw) << std::right << "\033[1;31mError(s)! See below.\033[0m\n ";
		auto msgs = builder.GetErrorMessages();	
		for(auto& msg : msgs)
			std::cout << "  " << msg << std::endl;

		return -1;
	}
	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";

	// Parse world.
	std::cout << std::setw(msgw) << std::left << " > Validating World...";
	if(!builder.ParseWorld())
	{
		std::cout << std::setw(notw) << std::right << "\033[1;31mError(s)! See below.\033[0m\n";
		return DumpErrorsToConsole(builder.GetErrorMessages());
	}
	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";
	DumpNoticesToConsole(builder.GetNotices(), "  ");
	builder.ResetNotices();

	// Initialize world.
	std::cout << std::setw(msgw) << std::left << " > Initializing World...";
	World* world = nullptr;
	world = builder.BuildWorld();
	if(world == nullptr)
	{
		std::cout << std::setw(notw) << std::right << "\033[1;31mError(s)! See below.\033[0m\n";
		delete world;
		return DumpErrorsToConsole({"Unable to initialize world. Unknown error occured!"});
	}

	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";
	DumpNoticesToConsole(builder.GetNotices(), "  ");
	builder.ResetNotices();

	// Parse particles.
	std::cout << std::setw(msgw) << std::left << " > Validating Particles...";
	if(!builder.ParseParticles())
	{
		std::cout << std::setw(notw) << std::right << "\033[1;31mError(s)! See below.\033[0m\n";
		delete world;
		return DumpErrorsToConsole(builder.GetErrorMessages());
	}
	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";
	DumpNoticesToConsole(builder.GetNotices(), "  ");
	builder.ResetNotices();

	// Build particles.
	std::cout << std::setw(msgw) << std::left << " > Building Particles...";
	builder.BuildParticles(world);
	std::cout << std::setw(notw) << std::right << "\033[32mOK!\033[0m\n";
	DumpNoticesToConsole(builder.GetNotices(), "  ");
	builder.ResetNotices();

	delete world;
	return 0;
}