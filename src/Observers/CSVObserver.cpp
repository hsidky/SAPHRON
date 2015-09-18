#include <iterator>
#include <iomanip>
#include "CSVObserver.h"
#include "../Particles/Particle.h"
#include "../Ensembles/Ensemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Worlds/WorldManager.h"
#include "../Worlds/World.h"
#include "../Histogram.h"

using namespace std;

namespace SAPHRON
{
	CSVObserver::CSVObserver(string prefix, SimFlags flags, unsigned int frequency) : 
		SimObserver(flags, frequency), _worldfs(0), _particlefs(), _dlm(" "), _ext(".csv"), 
		_fixl(true), _w(15), _prefix(prefix), _printedH(false)
	{
	}

	void CSVObserver::InitializeEnsemble(const Ensemble& e)
	{
		if(!this->Flags.simulation)
			return;

		// Open file for writing. 
		_simfs = unique_ptr<ofstream>(
			new ofstream(_prefix + ".simulation" + _ext)
			);
		_simfs->precision(8);
		*_simfs << scientific;


		if(this->Flags.iteration)
			*_simfs << setw(_w) << "Iteration" << _dlm;
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				*_simfs << setw(_w) << acceptance.first + " Acc." << _dlm;

		*_simfs << endl;
	}

	void CSVObserver::InitializeDOSEnsemble(const DOSEnsemble &e)
	{
		if(!this->Flags.simulation)
			return;

		// Open file for writing. 
		_simfs = unique_ptr<ofstream>(
			new ofstream(_prefix + ".simulation" + _ext)
			);
		_simfs->precision(8);
		*_simfs << scientific;


		if(this->Flags.iteration)
			*_simfs << setw(_w) << "Iteration" << _dlm;
		if(this->Flags.dos_factor)
			*_simfs << setw(_w) << "DOS Factor" << _dlm;
		if(this->Flags.dos_flatness)
			*_simfs << setw(_w) << "DOS Flatness" << _dlm;
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				*_simfs << setw(_w) << acceptance.first + " Acc." << _dlm;

		*_simfs << endl;
	}

	void CSVObserver::InitializeWorlds(const WorldManager& wm)
	{
		InitializeParticles(wm);
		if(!this->Flags.world)
			return;

		// Open files for writing.
		for(const auto& w : wm)
		{
			_worldfs.push_back(unique_ptr<ofstream>(
				new ofstream(_prefix + "." + w->GetStringID() + ".world" + _ext)
			));

			_worldfs.back()->precision(8);
			*_worldfs.back() << scientific;

			if(this->Flags.iteration)
				*_worldfs.back() << setw(_w) <<  "Iteration" << _dlm;
			if(this->Flags.world_temperature)
				*_worldfs.back() << setw(_w) << "Temperature" << _dlm;
			if(this->Flags.world_pressure)
				*_worldfs.back() << setw(_w) << "Pressure" << _dlm;
			if(this->Flags.world_volume)
				*_worldfs.back() << setw(_w) << "Volume" << _dlm;
			if(this->Flags.world_density)
				*_worldfs.back() << setw(_w) << "Density" << _dlm;
			if(this->Flags.world_energy)
				*_worldfs.back() << setw(_w) << "Energy" << _dlm;
			if(this->Flags.world_composition)
				for(auto&c : w->GetComposition())
					*_worldfs.back() << setw(_w) << "#" + to_string(c.first) << _dlm;

			*_worldfs.back() << "\n";
		}
	}

	void CSVObserver::InitializeParticles(const WorldManager& wm)
	{
		if(!this->Flags.particle)
			return;

		for(const auto& w : wm)
		{
			_particlefs[w->GetID()] = unique_ptr<ofstream>(
				new ofstream(_prefix + "." + w->GetStringID() + ".particle" + _ext)
			);

			auto& fs = _particlefs[w->GetID()];
			fs->precision(8);	

			if(this->Flags.iteration)
				*fs << setw(_w) <<  "Iteration" << _dlm;
			if(this->Flags.particle_id)
				*fs << setw(_w) << "ID" << _dlm;
			if(this->Flags.particle_species)
				*fs << setw(_w) << "Species" << _dlm;
			if(this->Flags.particle_parent_id)
				*fs << setw(_w) << "Parent ID" << _dlm;
			if(this->Flags.particle_parent_species) 
				*fs << setw(_w) << "Parent Species" << _dlm;
			if(this->Flags.particle_position)
				*fs << setw(_w) << "x" << _dlm << setw(_w)  << "y" << _dlm << setw(_w) << "z" << _dlm;
			if(this->Flags.particle_director)
				*fs << setw(_w) << "ux" << _dlm << setw(_w) << "uy" << _dlm << setw(_w) << "uz" << _dlm;
			if(this->Flags.particle_neighbors)
				*fs << setw(_w) << "Neighbor IDs" << _dlm;

			*fs << endl;
		}
	}

	void CSVObserver::InitializeHistogram(const Histogram& hist)
	{
		if(!this->Flags.histogram)
			return; 

		// Open file for writing. 
		_histfs = unique_ptr<ofstream>(
			new ofstream(_prefix + ".histogram" + _ext)
			);
		_histfs->precision(10);
		*_histfs << scientific;

		if(this->Flags.iteration)
			*_histfs << setw(_w) << "Iteration" << _dlm;
		if(this->Flags.hist_interval)
		{
			*_histfs << setw(_w) << "Minimum" << _dlm;
			*_histfs << setw(_w) << "Maximum" << _dlm;
		}
		if(this->Flags.hist_bin_count)
			*_histfs << setw(_w) << "Bin Count" << _dlm;
		if(this->Flags.hist_lower_outliers)
			*_histfs << setw(_w) << "Lower Outliers" << _dlm;
		if(this->Flags.hist_upper_outliers)
			*_histfs << setw(_w) << "Upper Outliers" << _dlm;
		if(this->Flags.hist_values)
		{
			auto min = hist.GetMinimum();
			auto bw = hist.GetBinWidth();
			for(int i = 0; i < hist.GetBinCount(); ++i)
				*_histfs << setw(_w) << to_string(min + i*bw) << _dlm;
		}
		
		*_histfs << endl;
	}

	void CSVObserver::Visit(const Ensemble& e)
	{
		// Flags sim is checked in init ensemble.
		if(!_printedH)
			InitializeEnsemble(e);

		if(!this->Flags.simulation)	
			return;

		if(this->Flags.iteration)
			*_simfs << setw(_w) << e.GetIteration() << _dlm;
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				*_simfs << setw(_w) << acceptance.second << _dlm;
	}

	void CSVObserver::Visit(const DOSEnsemble& e)
	{
		if(!_printedH)
			InitializeDOSEnsemble(e);

		if(!this->Flags.simulation)	
			return;

		if(this->Flags.iteration)
			*_simfs << setw(_w) << e.GetIteration() << _dlm;
		if(this->Flags.dos_factor)
			*_simfs << setw(_w) << e.GetConvergenceFactor() << _dlm;
		if(this->Flags.dos_flatness)
			*_simfs << setw(_w) << e.GetFlatness() << _dlm;
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				*_simfs << setw(_w) << acceptance.second << _dlm;
	}

	void CSVObserver::Visit(const Histogram& hist)
	{
		if(!_printedH)
			InitializeHistogram(hist);

		if(!this->Flags.histogram)
			return;

		if(this->Flags.iteration)
			*_histfs << setw(_w) << this->GetIteration() << _dlm;
		if(this->Flags.hist_interval)
		{
			*_histfs << setw(_w) << hist.GetMinimum() << _dlm;
			*_histfs << setw(_w) << hist.GetMaximum() << _dlm;
		}
		if(this->Flags.hist_bin_count)
			*_histfs << setw(_w) << hist.GetBinCount() << _dlm;
		if(this->Flags.hist_lower_outliers)
			*_histfs << setw(_w) << hist.GetLowerOutlierCount() << _dlm;
		if(this->Flags.hist_upper_outliers)
			*_histfs << setw(_w) << hist.GetUpperOutlierCount() << _dlm;
		if(this->Flags.hist_values)
			for(const auto& v : hist.GetValues())
				*_histfs << setw(_w) << v << _dlm;
	}

	void CSVObserver::Visit(const WorldManager &wm)
	{
		if(!_printedH)
			InitializeWorlds(wm);

		// Worlds flag is checked in init worlds. Keep first because 
		// it loads particle files as well.
		if(!this->Flags.world)
			return;

		int i = 0;
		for(auto& w : wm)
		{
			auto& fs = _worldfs[i];
			if(this->Flags.iteration)
				*fs << setw(_w) << fixed << this->GetIteration() << scientific << _dlm;
			if(this->Flags.world_temperature)
				*fs << setw(_w) << w->GetTemperature() << _dlm;
			if(this->Flags.world_pressure)
				*fs << setw(_w) << w->GetPressure().isotropic() << _dlm;
			if(this->Flags.world_volume)
				*fs << setw(_w) << w->GetVolume() << _dlm;
			if(this->Flags.world_density)
				*fs << setw(_w) << w->GetDensity() << _dlm;
			if(this->Flags.world_energy)
				*fs << setw(_w) << w->GetEnergy().total() << _dlm;
			if(this->Flags.world_composition)
				for(auto&c : w->GetComposition())
					*fs << setw(_w) << fixed << c.second << scientific << _dlm;
			++i;
		}
	}

	void CSVObserver::Visit(const MoveManager&)
	{
	}

	void CSVObserver::Visit(const ForceFieldManager&)
	{
	}

	void CSVObserver::Visit(const Particle& p)
	{
		if(!this->Flags.particle)
			return;

		auto& fs = _particlefs[p.GetWorld()->GetID()];
		if(this->Flags.iteration)
			*fs << setw(_w) <<  this->GetIteration() << _dlm;
		if(this->Flags.particle_id)
			*fs << setw(_w) << p.GetGlobalIdentifier() << _dlm;
		if(this->Flags.particle_species)
			*fs << setw(_w) << p.GetSpecies() << _dlm;
		if(this->Flags.particle_parent_id && p.HasParent())
			*fs << setw(_w) << p.GetParent()->GetGlobalIdentifier() << _dlm;
		if(this->Flags.particle_parent_species && p.HasParent()) 
			*fs << setw(_w) << p.GetParent()->GetSpecies() << _dlm;
		if(this->Flags.particle_position) 
		{
			const auto& pos = p.GetPositionRef();
			*fs << scientific; 
			*fs << setw(_w) << pos.x << _dlm;
			*fs << setw(_w) << pos.y << _dlm;
			*fs << setw(_w) << pos.z << _dlm;
			*fs << fixed;
		}
		if(this->Flags.particle_director) 
		{
			const auto& dir = p.GetDirectorRef();
			*fs << scientific; 
			*fs << setw(_w) << dir.x << _dlm;
			*fs << setw(_w) << dir.y << _dlm;
			*fs << setw(_w) << dir.z << _dlm;
			*fs << fixed;		
		}
		if(this->Flags.particle_neighbors)
		{
			for(auto& neighbor : p.GetNeighbors())
				*fs << setw(_w) << neighbor->GetGlobalIdentifier() << _dlm;		
		}

		*fs << "\n";
	}
}