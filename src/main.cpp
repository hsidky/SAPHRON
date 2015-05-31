#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "Worlds/World.h"
#include "Worlds/SimpleWorld.h"
#include "Particles/Site.h"
#include "Simulation/SimBuilder.h"
#include "ForceFields/ForceFieldManager.h"
#include "Connectivities/Connectivity.h"
#include "Moves/MoveManager.h"
#include "Simulation/SimObserver.h"
#include "DensityOfStates/DOSOrderParameter.h"
#include "Ensembles/Ensemble.h"

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

	//******************************************
	//                                         *
	//      BEGIN PARSING AND VALIDATION       *
	//                                         *
	//******************************************

	// Validate JSON.	
	std::cout << std::setw(msgw) << std::left << " > Validating JSON...";
	SimBuilder builder; 
	if(!builder.ParseInput(std::cin))
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole({}, "", notw);

	// Parse world.
	std::cout << std::setw(msgw) << std::left << " > Validating World...";
	if(!builder.ParseWorld())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse particles.
	std::cout << std::setw(msgw) << std::left << " > Validating Particles...";
	if(!builder.ParseParticles())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse connectivities.
	std::cout << std::setw(msgw) << std::left << " > Validating Connectivities...";
	if(!builder.ParseConnectivities())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse forcefields.
	std::cout << std::setw(msgw) << std::left << " > Validating ForceFields...";
	if(!builder.ParseForceFields())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse moves.
	std::cout << std::setw(msgw) << std::left << " > Validating Moves...";
	if(!builder.ParseMoves())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse observers.
	std::cout << std::setw(msgw) << std::left << " > Validating Observers...";
	if(!builder.ParseObservers())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Parse ensemble.
	std::cout << std::setw(msgw) << std::left << " > Validating Ensemble...";
	if(!builder.ParseEnsemble())
		return DumpErrorsToConsole(builder.GetErrorMessages(), notw);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	//******************************************
	//                                         *
	//        END PARSING AND VALIDATION       *
	//                                         *
	//******************************************



	//******************************************
	//                                         *
	//           BEGIN INITIALIZATION          *
	//                                         *
	//******************************************

	// Initialize world.
	std::cout << std::setw(msgw) << std::left << " > Initializing World...";
	World* world = nullptr;
	world = builder.BuildWorld();
	if(world == nullptr)
	{
		delete world;
		return DumpErrorsToConsole({"Unable to initialize world. Unknown error occurred!"}, notw);
	}
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Build particles.
	std::vector<Connectivity*> connectivities;
	std::cout << std::setw(msgw) << std::left << " > Building Particles...";
	builder.BuildParticles(world, connectivities);
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Initialize forcefields.
	ForceFieldManager ffm;
	std::vector<ForceField*> forcefields(0);
	std::cout << std::setw(msgw) << std::left << " > Building ForceFields...";
	builder.BuildForceFields(forcefields, ffm);
	if((int)forcefields.size() == 0)
	{
		for ( auto& cc : connectivities ) delete cc;
		connectivities.clear();
		delete world;
		return DumpErrorsToConsole({"Unable to initialize forcefields. Unknown error occurred."}, notw);
	}
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Initialize moves.
	MoveManager mm;
	std::vector<Move*> moves(0);
	std::cout << std::setw(msgw) << std::left << " > Building Moves...";
	builder.BuildMoves(moves, mm);
	if((int)moves.size() == 0)
	{
		for ( auto& cc : connectivities ) delete cc;
		connectivities.clear();
		for ( auto& ff : forcefields ) delete ff;
		forcefields.clear();
		delete world;
		return DumpErrorsToConsole({"Unable to initialize moves. Unknown error occurred."}, notw);
	}
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	// Initialize observers.
	std::vector<SimObserver*> observers(0);
	std::cout << std::setw(msgw) << std::left << " > Initializing Observers...";
	builder.BuildObservers(observers);
	if((int)moves.size() == 0)
	{
		for ( auto& cc : connectivities ) delete cc;
		connectivities.clear();
		for ( auto& ff : forcefields ) delete ff;
		forcefields.clear();
		for ( auto& m : moves ) delete m;
		moves.clear();
		delete world;
		return DumpErrorsToConsole({"Unable to initialize observers. Unknown error occurred."}, notw);
	}
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	Ensemble* ensemble = nullptr; 
	DOSOrderParameter* op = nullptr;
	int sweeps = 0;
	std::cout << std::setw(msgw) << std::left << " > Initializing Ensemble...";
	ensemble = builder.BuildEnsemble(*world, ffm, mm, observers, op, sweeps);
	if(ensemble == nullptr)
	{
		for ( auto& cc : connectivities ) delete cc;
		connectivities.clear();
		for ( auto& ff : forcefields ) delete ff;
		forcefields.clear();
		for ( auto& m : moves ) delete m;
		moves.clear();
		for (auto& oo : observers) delete oo;
		observers.clear();
		delete op;
		delete world;
		return DumpErrorsToConsole({"Unable to initialize ensemble. Unknown error occurred."}, notw);
	}
	DumpNoticesToConsole(builder.GetNotices(), "  ", notw);
	builder.ResetNotices();

	//******************************************
	//                                         *
	//             END INITIALIZATION          *
	//                                         *
	//******************************************

	// Run simulation.
	ensemble->Run(sweeps);

	// Cleanup.
	for ( auto& ff : forcefields ) delete ff;
	forcefields.clear();

	for ( auto& cc : connectivities ) delete cc;
	connectivities.clear();

	for ( auto& m : moves ) delete m;
	moves.clear();

	for (auto& oo : observers) delete oo;
	observers.clear();

	delete op;
	delete ensemble; 
	delete world;
	return 0;
}	