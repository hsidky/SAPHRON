#include <iterator>
#include "CSVObserver.h"
#include "../Particles/Particle.h"
#include "../Ensembles/Ensemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Worlds/World.h"

namespace SAPHRON
{

	CSVObserver::CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency) : 
		SimObserver(flags, frequency), _printedE(0)
	{
		if(flags.ensemble)
		{
			_ensemblefs = std::unique_ptr<std::ofstream>(
				new std::ofstream(prefix + ".ensemble.csv")
				);
			_ensemblefs->precision(20);
		}

		if(flags.dos)
		{
			_dosfs = std::unique_ptr<std::ofstream>(
                new std::ofstream(prefix + ".dos.csv")
                );
			_dosfs->precision(20);

			if(flags.identifier)
				*_dosfs << "Identifier,";
			if(flags.iterations)
				*_dosfs << "Iterations,";
			if(flags.dos_walker)
				*_dosfs << "Walker ID,";
			if(flags.dos_scale_factor)
				*_dosfs << "Scale Factor,";
			if(flags.dos_flatness)
				*_dosfs << "Flatness,";
			if(flags.dos_bin_count)
				*_dosfs << "Bin Count,";
			if(flags.dos_interval)
				*_dosfs << "Interval Min,Interval Max,";
			if(flags.dos_values)
				*_dosfs << "DOS,";

			*_dosfs << std::endl;
		}

		if(flags.particle)
		{
			_particlefs = std::unique_ptr<std::ofstream>(
				new std::ofstream(prefix + ".particle.csv")
				);
			_particlefs->precision(20);
			if (this->Flags.identifier)
            	*_particlefs << "Identifier,";
         	if (this->Flags.iterations)
                *_particlefs << "Iterations,";
            if(this->Flags.particle_global_id)
            	*_particlefs << "Global ID,";
         	if(this->Flags.particle_species)
                *_particlefs << "Species,";
            if(this->Flags.particle_species_id)
                *_particlefs << "Species ID,";
         	if(this->Flags.particle_position)
                *_particlefs << "X,Y,Z,";
         	if(this->Flags.particle_director)
                *_particlefs << "UX,UY,UZ,";
         	if(this->Flags.particle_neighbors)
                *_particlefs << "Neighbor List,";

         	*_particlefs << std::endl;

		}
	}

	void CSVObserver::PrintEnsembleHeader(Ensemble* e)
	{
		if(this->Flags.identifier)
			*_ensemblefs << "Identifier,";
		if(this->Flags.iterations)
			*_ensemblefs << "Iterations,";
		if(this->Flags.energy)
			*_ensemblefs << "Non-bonded Energy,Connectivity Energy,Total Energy,";
		if(this->Flags.temperature)
			*_ensemblefs << "Temperature,";
		if(this->Flags.pressure)
			*_ensemblefs << "Pressure,";
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				*_ensemblefs << acceptance.first + " Acceptance,";

		*_ensemblefs << std::endl;
	}

	void CSVObserver::Visit(Ensemble *e)
	{
		// Write header.
		if(!_printedE)
		{
			PrintEnsembleHeader(e);
			_printedE = true;
		}

		if(this->Flags.identifier)
			*_ensemblefs << this->GetObservableID() << ",";
		if(this->Flags.iterations)
			*_ensemblefs << this->GetIteration() << ",";
		if(this->Flags.energy)
		{
			auto energy = e->GetEnergy();
			*_ensemblefs << energy.nonbonded << "," 
						 << energy.connectivity << ","
						 << energy.total() << ",";
		}
		if(this->Flags.temperature)
			*_ensemblefs << e->GetTemperature() << ",";
		if(this->Flags.pressure)
			*_ensemblefs << e->GetPressure() << ",";
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				*_ensemblefs << acceptance.second << ",";

		*_ensemblefs << std::endl;
	}

	void CSVObserver::Visit(DOSEnsemble *e)
	{
		// Visit ensemble.
		Visit((Ensemble*) e);

		if(this->Flags.identifier)
				*_dosfs << this->GetObservableID() << ",";
		if(this->Flags.iterations)
			*_dosfs << this->GetIteration() << ",";
		if(this->Flags.dos_walker) // Temporary until walkers are implemented.
			*_dosfs << "Walker ID,";
		if(this->Flags.dos_scale_factor)
			*_dosfs << e->GetScaleFactor() << ",";
		if(this->Flags.dos_flatness)
			*_dosfs << e->GetFlatness() << ",";
		if(this->Flags.dos_bin_count)
			*_dosfs << e->GetBinCount() << ",";
		if(this->Flags.dos_interval)
		{
			auto interval = e->GetInterval();
			*_dosfs << interval.first << "," << interval.second << ",";
		}
		if(this->Flags.dos_values)
		{
			auto *dos = e->GetDensityOfStates();
			std::copy(dos->begin(), dos->end(), std::ostream_iterator<double>(*_dosfs, ","));
		}

		*_dosfs << std::endl;
	}

	void CSVObserver::Visit(World*)
	{

	}

	void CSVObserver::Visit(Particle *p)
	{
		if(!this->Flags.particle)
			return;

		if(this->Flags.identifier)
			*_particlefs << this->GetObservableID() << ",";
		if(this->Flags.iterations)
			*_particlefs << this->GetIteration() << ",";
		if(this->Flags.particle_global_id)
			*_particlefs << p->GetGlobalIdentifier() << ",";
		if(this->Flags.particle_species)
			*_particlefs << p->GetSpecies() << ",";
		if(this->Flags.particle_species_id)
			*_particlefs << p->GetSpeciesID() << ",";
		if (this->Flags.particle_position)
	    {
	    	auto coords = p->GetPosition();
	    	*_particlefs << coords.x  << "," <<  coords.y << "," << coords.z << ",";
		}
		if (this->Flags.particle_director)
	    {
	    	auto& dir = p->GetDirectorRef();
	    	*_particlefs << dir[0] << "," << dir[1] << "," << dir[2] << ",";
	    }
	    if (this->Flags.particle_neighbors)
	    {
	    	auto& neighbors = p->GetNeighbors();
	        for(auto& neighbor : neighbors)
	        	*_particlefs << neighbor->GetGlobalIdentifier() << ",";
	   	}

	   	*_particlefs << std::endl;
	}
}