#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Simulation/SimBuilder.h"
#include "config.h"

using namespace SAPHRON;

#ifdef MULTI_WALKER
#include <boost/mpi.hpp>
using namespace boost;
#endif

int main(int argc, char* argv[])
{
	#ifdef MULTI_WALKER
	mpi::environment env(argc, argv);
	mpi::communicator comm;
	if(comm.rank() == 0)
	{
	#endif

  	std::string rev(GIT_SHA1, strnlen(GIT_SHA1, 40));
  	std::string revision;

  	if (!rev.empty())
  		revision.assign(" Revision: " + rev );
  
	revision.resize(53,' ');

	std::cout << "                                                                         \n" << 
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
#ifdef MULTI_WALKER
	             " *                    MPI multi-walker DOS edition                    *\n" <<
#endif
	             " **********************************************************************      \n" << 
	             "                                                                         \n";

	#ifdef MULTI_WALKER
	}
	#endif

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
		#ifdef MULTI_WALKER
		if(comm.rank() == 0)
		#endif
		std::cout << std::setw(47 + 8) << std::left << "\033[1m > Running simulation... \033[0m" << std::flush;
	
		try{
			auto* sim = builder.GetSimulation();
			sim->Run();
		}catch(std::exception& e){
			#ifdef MULTI_WALKER
			if(comm.rank() == 0)
			{
			#endif

			std::cout << std::setw(30) << std::right << "\033[1;31mError(s)! See below.\033[0m\n";
			std::cout << " * " << e.what() << std::endl;

			#ifdef MULTI_WALKER
			}
			env.abort(-1);
			#endif
		}
		
		#ifdef MULTI_WALKER
		if(comm.rank() == 0)
		{
		#endif
		std::cout << std::setw(34) << std::right << "\033[32mComplete!\033[0m\n";
		
		// Time Info. 
		auto& info = SimInfo::Instance();
		info.AddTime("total");

		auto& map = info.GetTimerMap();
		auto tot = map.at("total").elapsed_time;
		std::cout << " * Elapsed time breakdown during simulation:\n";
		std::cout << std::fixed << std::setprecision(2);
		for(auto& it : map)
		{
			auto t = it.second.elapsed_time;
			std::cout << " * " << info.ResolveTimerName(it.first) 
					  << ": " << std::chrono::duration_cast<std::chrono::seconds>(t).count() << " s" 
					  << " (" << (double)t.count()/(double)tot.count()*100. << "%)" << std::endl;
		}
		#ifdef MULTI_WALKER
		}
		#endif
	}

	#ifdef MULTI_WALKER
	if(comm.rank() == 0)
		env.abort(0);
	#endif

	return 0;
}
