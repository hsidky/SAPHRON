#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Simulation/SimBuilder.h"
#include "config.h"

using namespace SAPHRON;

int main(int argc, char const* argv[])
{

  	std::string rev(GIT_SHA1, strnlen(GIT_SHA1, 40));
  	std::string revision;

  	if (!rev.empty())
  		revision.assign(" Revision: " + rev );
  
	revision.resize(53,' ');

	std::cerr << "                                                                         \n" << 
	             " **********************************************************************      \n" << 
	             " *         ____      _     ____   _   _  ____    ___   _   _          *      \n" << 
	             " *        / ___|    / \\   |  _ \\ | | | ||  _ \\  / _ \\ | \\ | |         * \n" << 
	             " *        \\___ \\   / _ \\  | |_) || |_| || |_) || | | ||  \\| |         *  \n" << 
	             " *         ___) | / ___ \\ |  __/ |  _  ||  _ < | |_| || |\\  |         *    \n" << 
	             " *        |____/ /_/   \\_\\|_|    |_| |_||_| \\_\\ \\___/ |_| \\_|         *\n" << 
	             " *                                                                    *      \n" << 
	             " *   \033[1mS\033[0mtatistical \033[1mA\033[0mpplied \033[1mPH\033[0mysics "   <<
	             "through \033[1mR\033[0mandom \033[1mO\033[0mn-the-fly \033[1mN\033[0mumerics   *\n" << 
	             " *                                                                    *     \n" << 
	             " *  Version " << SAPHRON_VERSION_MAJOR << "." << 
	             			   SAPHRON_VERSION_MINOR << "." << 
	             			   SAPHRON_VERSION_TINY  << revision << "*      \n" << 
	             " **********************************************************************      \n" << 
	             "                                                                         \n";

	int validate_only = 0;
	std::string filename;
	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-v") == 0)
			validate_only = 1;
		else
			filename = argv[i];
	}

	SimBuilder builder;
	if(!builder.BuildSimulation(filename))
		return -1;

	if(validate_only == 0)
	{
		std::cerr << std::setw(47 + 8) << std::left << "\033[1m > Running simulation... \033[0m";
		auto* sim = builder.GetSimulation();
		sim->Run();
		std::cerr << std::setw(34) << std::right << "\033[32mComplete!\033[0m\n";
		
		// Time Info. 
		auto& info = SimInfo::Instance();
		info.AddTime("total");

		auto& map = info.GetTimerMap();
		auto tot = map.at("total").elapsed_time.count();
		std::cerr << " * Elapsed time breakdown during simulation:\n";
		std::cerr << std::fixed << std::setprecision(2);
		for(auto& it : map)
		{
			auto t = it.second.elapsed_time.count();
			std::cerr << " * " << info.ResolveTimerName(it.first) 
					  << ": " << t << " ms" 
					  << " (" << (float)t/tot*100. << "%)" << std::endl;
		}
	}

	return 0;
}
