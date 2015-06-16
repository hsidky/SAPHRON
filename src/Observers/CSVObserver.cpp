#include <iterator>
#include <iomanip>
#include "CSVObserver.h"
#include "../Particles/Particle.h"
#include "../Ensembles/Ensemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Worlds/World.h"

namespace SAPHRON
{

	CSVObserver::CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency) : 
		SimObserver(flags, frequency), _dlm(","), _ext(".csv"), _printedE(false), _printedW(false)
	{
		if(flags.ensemble)
		{
			_ensemblefs = std::unique_ptr<std::ofstream>(
				new std::ofstream(prefix + ".ensemble" + _ext)
				);
			_ensemblefs->precision(15);
			*_ensemblefs << std::scientific;
		}

		if(flags.dos)
		{
			_dosfs = std::unique_ptr<std::ofstream>(
                new std::ofstream(prefix + ".dos" + _ext)
                );
			_dosfs->precision(15);

			if(flags.identifier)
				*_dosfs << "Identifier" << _dlm;
			if(flags.iterations)
				*_dosfs << "Iterations" << _dlm;
			if(flags.dos_walker)
				*_dosfs << "Walker ID" << _dlm;
			if(flags.dos_scale_factor)
				*_dosfs << "Scale Factor" << _dlm;
			if(flags.dos_flatness)
				*_dosfs << "Flatness" << _dlm;
			if(flags.dos_bin_count)
				*_dosfs << "Bin Count" << _dlm;
			if(flags.dos_interval)
			{
				*_dosfs << "Interval Min" << _dlm;
				*_dosfs << "Interval Max" << _dlm;
			}
			if(flags.dos_values)
				*_dosfs << "DOS" << _dlm;

			*_dosfs << std::endl;
		}

		if(flags.world)
		{
			_worldfs = std::unique_ptr<std::ofstream>(
                new std::ofstream(prefix + ".world" + _ext)
                );
			_worldfs->precision(15);

			if(flags.world_count)
				*_worldfs << "Particle Count" << _dlm;
			if(flags.world_density)
				*_worldfs << "Density" << _dlm;
			if(flags.world_volume)
				*_worldfs << "Volume" << _dlm;

			*_worldfs << std::endl;
		}

		if(flags.particle)
		{
			_particlefs = std::unique_ptr<std::ofstream>(
				new std::ofstream(prefix + ".particle" + _ext)
				);
			_particlefs->precision(15);

			if (this->Flags.identifier)
            	*_particlefs << "Identifier" << _dlm;
         	if (this->Flags.iterations)
                *_particlefs << "Iterations" << _dlm;
            if(this->Flags.particle_global_id)
            	*_particlefs << "Global ID" << _dlm;
         	if(this->Flags.particle_species)
                *_particlefs << "Species" << _dlm;
            if(this->Flags.particle_species_id)
                *_particlefs << "Species ID" << _dlm;
         	if(this->Flags.particle_position)
                *_particlefs << "X,Y,Z" << _dlm;
         	if(this->Flags.particle_director)
         	{
                *_particlefs << "UX" << _dlm;
                *_particlefs << "UY" << _dlm;
                *_particlefs << "UZ" << _dlm;
         	}
         	if(this->Flags.particle_neighbors)
                *_particlefs << "Neighbor List" << _dlm;

         	*_particlefs << std::endl;

		}
	}

	void CSVObserver::PrintEnsembleHeader(Ensemble* e)
	{
		if(this->Flags.identifier)
			*_ensemblefs << "Identifier" << _dlm;
		if(this->Flags.iterations)
			*_ensemblefs << "Iterations" << _dlm;
		if(this->Flags.energy)
		{
			*_ensemblefs << "Non-bonded Energy" << _dlm;
			*_ensemblefs << "Connectivity Energy" << _dlm;
			*_ensemblefs << "Total Energy" << _dlm;
		}
		if(this->Flags.temperature)
			*_ensemblefs << "Temperature" << _dlm;
		if(this->Flags.pressure)
			*_ensemblefs << "Pressure" << _dlm;
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				*_ensemblefs << acceptance.first + " Acceptance" << _dlm;

		*_ensemblefs << std::endl;
	}

	void CSVObserver::Visit(Ensemble *e)
	{
		// Write header.
		if(!_printedE)
			PrintEnsembleHeader(e);

		if(this->Flags.identifier)
			*_ensemblefs << this->GetObservableID() << _dlm;
		if(this->Flags.iterations)
			*_ensemblefs << this->GetIteration() << _dlm;
		if(this->Flags.energy)
		{
			auto energy = e->GetEnergy();
			*_ensemblefs << energy.nonbonded << _dlm;
			*_ensemblefs << energy.connectivity << _dlm;
			*_ensemblefs << energy.total() << _dlm;
		}
		if(this->Flags.temperature)
			*_ensemblefs << e->GetTemperature() << _dlm;
		if(this->Flags.pressure)
			*_ensemblefs << e->GetPressure().isotropic() << _dlm;
		if(this->Flags.acceptance)
			for(auto& acceptance : e->GetAcceptanceRatio())
				*_ensemblefs << acceptance.second << _dlm;

		*_ensemblefs << std::endl;
	}

	void CSVObserver::Visit(DOSEnsemble *e)
	{
		// Visit ensemble.
		Visit((Ensemble*) e);

		if(this->Flags.identifier)
			*_dosfs << this->GetObservableID() << _dlm;
		if(this->Flags.iterations)
			*_dosfs << this->GetIteration() << _dlm;
		if(this->Flags.dos_walker) // Temporary until walkers are implemented.
			*_dosfs << "TOBEIMPLEMENTED" << _dlm;
		if(this->Flags.dos_scale_factor)
			*_dosfs << e->GetScaleFactor() << _dlm;
		if(this->Flags.dos_flatness)
			*_dosfs << e->GetFlatness() << _dlm;
		if(this->Flags.dos_bin_count)
			*_dosfs << e->GetBinCount() << _dlm;
		if(this->Flags.dos_interval)
		{
			auto interval = e->GetInterval();
			*_dosfs << interval.first << _dlm;
			*_dosfs << interval.second << _dlm;
		}
		if(this->Flags.dos_values)
		{
			auto *dos = e->GetDensityOfStates();
			std::copy(dos->begin(), dos->end(), std::ostream_iterator<double>(*_dosfs, _dlm.c_str()));
		}

		*_dosfs << std::endl;
	}

	void CSVObserver::Visit(World* world)
	{
		if(this->Flags.world_count)
			*_worldfs << world->GetParticleCount() << _dlm;
		if(this->Flags.world_density)
			*_worldfs << world->GetDensity() << _dlm;
		if(this->Flags.world_volume)
			*_worldfs << world->GetVolume() << _dlm;
	}

	void CSVObserver::Visit(MoveManager*)
	{
	}

	void CSVObserver::Visit(ForceFieldManager*)
	{
	}

	void CSVObserver::Visit(Particle *p)
	{
		if(!this->Flags.particle)
			return;

		if(this->Flags.identifier)
			*_particlefs << this->GetObservableID() << _dlm;
		if(this->Flags.iterations)
			*_particlefs << this->GetIteration() << _dlm;
		if(this->Flags.particle_global_id)
			*_particlefs << p->GetGlobalIdentifier() << _dlm;
		if(this->Flags.particle_species)
			*_particlefs << p->GetSpecies() << _dlm;
		if(this->Flags.particle_species_id)
			*_particlefs << p->GetSpeciesID() << _dlm;
		if (this->Flags.particle_position)
	    {
	    	auto coords = p->GetPosition();
	    	*_particlefs << coords.x << _dlm;
	    	*_particlefs << coords.y << _dlm;  
	    	*_particlefs << coords.z << _dlm;
		}
		if (this->Flags.particle_director)
	    {
	    	auto& dir = p->GetDirectorRef();
	    	*_particlefs << dir.x << _dlm;
	    	*_particlefs << dir.y << _dlm;
	    	*_particlefs << dir.z << _dlm;
	    }
	    if (this->Flags.particle_neighbors)
	    {
	    	auto& neighbors = p->GetNeighbors();
	        for(auto& neighbor : neighbors)
	        	*_particlefs << neighbor->GetGlobalIdentifier() << _dlm;
	   	}

	   	*_particlefs << std::endl;
	}
}