#include "../Ensembles/Ensemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Particles/Particle.h"
#include "ConsoleObserver.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

using namespace std;

namespace SAPHRON
{
	void ConsoleObserver::Visit(Ensemble* e)
	{
		cout << endl << endl;
		if (this->Flags.iterations)
			cout << setw(20) << left << "Iteration";
		if (this->Flags.energy)
		{
			cout << setw(20) << left << "Non-bonded E";
			cout << setw(20) << left << "Connectivity E";
			cout << setw(20) << left << "Total E";
		}
		if (this->Flags.temperature)
			cout << setw(20) << left << "Temperature";
		if(this->Flags.pressure)
			cout << setw(20) << left << "Pressure";
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				cout << setw(20) << left << acceptance.first + " Acc.";
		cout << endl;
		
		
		if (this->Flags.iterations)
			cout << setw(20) << left << e->GetIteration();
		if (this->Flags.energy)
		{
			auto energy = e->GetEnergy();
			cout << setw(20) << setprecision(5) << left << scientific << energy.nonbonded;
			cout << setw(20) << setprecision(5) << left << scientific << energy.connectivity;
			cout << setw(20) << setprecision(5) << left << scientific << energy.total();
		}
		if (this->Flags.temperature)
			cout << setw(20) << left << e->GetTemperature();
		if(this->Flags.pressure)
			cout << setw(20) << left << e->GetPressure().isotropic();
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				cout << setw(20) << left << acceptance.second;
					 
		cout << endl;
	}

	void ConsoleObserver::Visit(DOSEnsemble* e)
	{
		Visit((Ensemble*) e);

		if(!this->Flags.dos)
			return;

		cout << endl;
		cout << setw(20) << "";
		if(this->Flags.dos_scale_factor)
			cout << setw(20) << left << "Scale Factor";
		if(this->Flags.dos_flatness)
			cout << setw(20) << left << "Flatness";
		if(this->Flags.dos_bin_count)
			cout << setw(20) << left << "Bin Count";
		if(this->Flags.dos_interval)
			cout << setw(20) << left << "Interval";
		cout << endl;

		cout << setw(20) << "";
		if(this->Flags.dos_scale_factor)
			cout << setw(20) << left << setprecision(5) << scientific <<  e->GetScaleFactor();
		if(this->Flags.dos_flatness)
			cout << setw(20) << left << setprecision(5) << fixed << e->GetFlatness();
		if(this->Flags.dos_bin_count)
			cout << setw(20) << left << e->GetBinCount();
		if(this->Flags.dos_interval)
		{
			auto interval = e->GetInterval();
			cout << setw(10) << fixed 
				 << setprecision(3) << left 
				 << interval.first << " " 
				 << interval.second;
		}

		cout << endl;
	}

	void ConsoleObserver::Visit(MoveManager*)
	{
	}

	void ConsoleObserver::Visit(ForceFieldManager*)
	{}

	void ConsoleObserver::Visit(World*)
	{
	}
	
	void ConsoleObserver::Visit(Particle* p)
	{
 		if(!this->Flags.particle)
	    	return;
		
		if(this->Flags.particle_global_id)
			cout << setw(10) << left << "Global ID";
		if(this->Flags.particle_species)
			cout << setw(10) << left << "Species";
		if(this->Flags.particle_species_id)
			cout << setw(10) << left << "Species ID";
		if(this->Flags.particle_position)
			cout << setw(42) << left << "Position";
		if(this->Flags.particle_director)
			cout << setw(42) <<  left << "Director";
		if(this->Flags.particle_neighbors)
			cout << setw(20) << left << "Neighbors";

		cout << endl;
		if(this->Flags.particle_global_id)
			cout << setw(10) << left << p->GetGlobalIdentifier();
		if(this->Flags.particle_species)
			cout << setw(10) << left << p->GetSpecies();
		if(this->Flags.particle_species_id)
			cout << setw(10) << left << p->GetSpeciesID();
		if (this->Flags.particle_position)
	    {
	    	auto coords = p->GetPosition();
	    	cout << fixed << setw(14) << setprecision(5) << left 
	    		 << coords.x  << setw(14) <<  coords.y << setw(14) << coords.z;
		}
		if (this->Flags.particle_director)
	    {
	    	auto& dir = p->GetDirectorRef();
	    	cout << left << setw(14) << dir.x << setw(14)  << dir.y << setw(14) << dir.z;
	    }
		if (this->Flags.particle_neighbors)
	    {
	    	auto& neighbors = p->GetNeighbors();
	        for(auto& neighbor : neighbors)
	           	cout << setw(20/neighbors.size()) << neighbor->GetGlobalIdentifier() << " ";
	   	}

	   	cout << endl;
	}
}
