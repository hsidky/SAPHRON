	#include <iterator>
#include <iomanip>
#include "DLMFileObserver.h"
#include "../Particles/Particle.h"
#include "../Simulation/Simulation.h"
#include "../Simulation/DOSSimulation.h"
#include "../Worlds/WorldManager.h"
#include "../Worlds/World.h"
#include "../Utils/Histogram.h"

using namespace std;

namespace SAPHRON
{
	template<typename T>
	void DLMFileObserver::WriteStream(ofstream& stream, const T& data)
	{
		if(_fixl)
			stream << setw(_w) << data << _dlm;
		else
			stream << data << _dlm;
	}	

	template<typename T>
	void DLMFileObserver::WriteStream(ofstream& stream, const T& data, int width)
	{
		if(_fixl)
			stream << setw(width) << data << _dlm;
		else
			stream << data << _dlm;
	}

	DLMFileObserver::DLMFileObserver(const string& prefix, 
									 const SimFlags& flags, 
									 unsigned int frequency) : 
	SimObserver(flags, frequency), _worldfs(0), _particlefs(),
	_dlm(" "), _ext(".dat"), _fixl(true), _w(15), _prefix(prefix), 
	_printedH(false)
	{
	}

	void DLMFileObserver::InitializeEnsemble(const Simulation& e)
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
			WriteStream(*_simfs,"Iteration");
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				WriteStream(*_simfs,acceptance.first + " Acc.");

		*_simfs << endl;
	}

	void DLMFileObserver::InitializeDOSSimulation(const DOSSimulation &e)
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
			WriteStream(*_simfs, "Iteration");
		if(this->Flags.dos_factor)
			WriteStream(*_simfs, "DOS Factor");
		if(this->Flags.dos_flatness)
			WriteStream(*_simfs, "DOS Flatness");
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				WriteStream(*_simfs, acceptance.first + " Acc.");

		*_simfs << endl;
	}

	void DLMFileObserver::InitializeWorlds(const WorldManager& wm)
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
				WriteStream(*_worldfs.back(),  "Iteration");
			if(this->Flags.world_temperature)
				WriteStream(*_worldfs.back(), "Temperature");
			if(this->Flags.world_pressure)
				WriteStream(*_worldfs.back(), "Pressure");
			if(this->Flags.pideal)
				WriteStream(*_worldfs.back(), "P ideal");
			if(this->Flags.pxx)
				WriteStream(*_worldfs.back(), "Pxx");
			if(this->Flags.pxy)
				WriteStream(*_worldfs.back(), "Pxy");
			if(this->Flags.pxz)
				WriteStream(*_worldfs.back(), "Pxz");
			if(this->Flags.pyy)
				WriteStream(*_worldfs.back(), "Pyy");
			if(this->Flags.pyz)
				WriteStream(*_worldfs.back(), "Pyz");
			if(this->Flags.pzz)
				WriteStream(*_worldfs.back(), "Pzz");
			if(this->Flags.ptail)
				WriteStream(*_worldfs.back(), "P tail");
			if(this->Flags.world_volume)
				WriteStream(*_worldfs.back(), "Volume");
			if(this->Flags.world_density)
				WriteStream(*_worldfs.back(), "Density");
			if(this->Flags.world_energy)
				WriteStream(*_worldfs.back(), "Energy");
			if(this->Flags.eintervdw)
				WriteStream(*_worldfs.back(), "E VDW Inter");
			if(this->Flags.eintravdw)
				WriteStream(*_worldfs.back(), "E VDW Intra");
			if(this->Flags.einterelect)
				WriteStream(*_worldfs.back(), "E Electro Inter");
			if(this->Flags.eintraelect)
				WriteStream(*_worldfs.back(), "E Electro Intra");
			if(this->Flags.ebonded)
				WriteStream(*_worldfs.back(), "E Bonded");
			if(this->Flags.econnectivity)
				WriteStream(*_worldfs.back(), "E Connectivity");
			if(this->Flags.world_composition)
				for(auto&c : w->GetComposition())
					WriteStream(*_worldfs.back(), "#" + to_string(c.first));

			*_worldfs.back() << "\n";
		}
	}

	void DLMFileObserver::InitializeParticles(const WorldManager& wm)
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
				WriteStream(*fs,  "Iteration");
			if(this->Flags.particle_id)
				WriteStream(*fs, "ID");
			if(this->Flags.particle_species)
				WriteStream(*fs, "Species");
			if(this->Flags.particle_parent_id)
				WriteStream(*fs, "Parent ID");
			if(this->Flags.particle_parent_species) 
				WriteStream(*fs, "Parent Species");
			if(this->Flags.particle_position)
			{
				WriteStream(*fs, "x");
				WriteStream(*fs, "y");
				WriteStream(*fs, "z");
			}
			if(this->Flags.particle_director)
			{
				WriteStream(*fs, "ux");
				WriteStream(*fs, "uy");
				WriteStream(*fs, "uz");
			}
			if(this->Flags.particle_neighbors)
				WriteStream(*fs, "Neighbor IDs");

			*fs << endl;
		}
	}

	void DLMFileObserver::InitializeHistogram(const Histogram& hist)
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
			WriteStream(*_histfs, "Iteration");
		if(this->Flags.hist_interval)
		{
			WriteStream(*_histfs, "Minimum");
			WriteStream(*_histfs, "Maximum");
		}
		if(this->Flags.hist_bin_count)
			WriteStream(*_histfs, "Bin Count");
		if(this->Flags.hist_lower_outliers)
			WriteStream(*_histfs, "Lower Outliers");
		if(this->Flags.hist_upper_outliers)
			WriteStream(*_histfs, "Upper Outliers");
		if(this->Flags.hist_values)
		{
			auto min = hist.GetMinimum();
			auto bw = hist.GetBinWidth();
			for(int i = 0; i < hist.GetBinCount(); ++i)
				WriteStream(*_histfs, to_string(min + i*bw));
		}
		
		*_histfs << endl;
	}

	void DLMFileObserver::Visit(const Simulation& e)
	{
		// Flags sim is checked in init ensemble.
		if(!_printedH)
			InitializeEnsemble(e);

		if(!this->Flags.simulation)	
			return;

		if(this->Flags.iteration)
			WriteStream(*_simfs, e.GetIteration());
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				WriteStream(*_simfs, acceptance.second);
	}

	void DLMFileObserver::Visit(const DOSSimulation& e)
	{
		if(!_printedH)
			InitializeDOSSimulation(e);

		if(!this->Flags.simulation)	
			return;

		if(this->Flags.iteration)
			WriteStream(*_simfs, e.GetIteration());
		if(this->Flags.dos_factor)
			WriteStream(*_simfs, e.GetConvergenceFactor());
		if(this->Flags.dos_flatness)
			WriteStream(*_simfs, e.GetFlatness());
		if(this->Flags.move_acceptances)
			for(auto& acceptance : e.GetAcceptanceRatio())
				WriteStream(*_simfs, acceptance.second);
	}

	void DLMFileObserver::Visit(const Histogram& hist)
	{
		if(!_printedH)
			InitializeHistogram(hist);

		if(!this->Flags.histogram)
			return;


		if(this->Flags.iteration)
			WriteStream(*_histfs, this->GetIteration());
		if(this->Flags.hist_interval)
		{
			WriteStream(*_histfs, hist.GetMinimum());
			WriteStream(*_histfs, hist.GetMaximum());
		}
		if(this->Flags.hist_bin_count)
			WriteStream(*_histfs, hist.GetBinCount());
		if(this->Flags.hist_lower_outliers)
			WriteStream(*_histfs, hist.GetLowerOutlierCount());
		if(this->Flags.hist_upper_outliers)
			WriteStream(*_histfs, hist.GetUpperOutlierCount());
		if(this->Flags.hist_values)
		{
			// We need high precision for histogram values.
			_histfs->precision(16);
			int w = 22;
			
			for(const auto& v : hist.GetValues())
				WriteStream(*_histfs, v, w);
			
			_histfs->precision(8);
		}
	}

	void DLMFileObserver::Visit(const WorldManager &wm)
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
			{
				*fs << fixed;
				WriteStream(*fs, this->GetIteration());
				*fs << scientific;
			}

			auto E = w->GetEnergy();
			auto P = w->GetPressure();
			
			if(this->Flags.world_temperature)
				WriteStream(*fs, w->GetTemperature());
			if(this->Flags.world_pressure)
				WriteStream(*fs, P.isotropic());
			if(this->Flags.pideal)
				WriteStream(*_worldfs.back(), P.ideal);
			if(this->Flags.pxx)
				WriteStream(*_worldfs.back(), P.pxx);
			if(this->Flags.pxy)
				WriteStream(*_worldfs.back(), P.pxy);
			if(this->Flags.pxz)
				WriteStream(*_worldfs.back(), P.pxz);
			if(this->Flags.pyy)
				WriteStream(*_worldfs.back(), P.pyy);
			if(this->Flags.pyz)
				WriteStream(*_worldfs.back(), P.pyz);
			if(this->Flags.pzz)
				WriteStream(*_worldfs.back(), P.pzz);
			if(this->Flags.ptail)
				WriteStream(*_worldfs.back(), P.ptail);
			if(this->Flags.world_volume)
				WriteStream(*fs, w->GetVolume());
			if(this->Flags.world_density)
				WriteStream(*fs, w->GetNumberDensity());
			if(this->Flags.world_energy)
				WriteStream(*fs, E.total());
			if(this->Flags.eintervdw)
				WriteStream(*fs, E.intervdw);
			if(this->Flags.eintravdw)
				WriteStream(*fs, E.intravdw);
			if(this->Flags.einterelect)
				WriteStream(*fs, E.interelectrostatic);
			if(this->Flags.eintraelect)
				WriteStream(*fs, E.intraelectrostatic);
			if(this->Flags.ebonded)
				WriteStream(*fs, E.bonded);
			if(this->Flags.econnectivity)
				WriteStream(*fs, E.connectivity);
			if(this->Flags.world_composition)
			{
				*fs << fixed;
				for(auto&c : w->GetComposition())
					WriteStream(*fs, c.second);
				*fs << scientific;
			}
			++i;
		}
	}

	void DLMFileObserver::Visit(const MoveManager&)
	{
	}

	void DLMFileObserver::Visit(const ForceFieldManager&)
	{
	}

	void DLMFileObserver::Visit(const Particle& p)
	{
		if(!this->Flags.particle)
			return;

		auto& fs = _particlefs[p.GetWorld()->GetID()];
		if(this->Flags.iteration)
			WriteStream(*fs,  this->GetIteration());
		if(this->Flags.particle_id)
			WriteStream(*fs, p.GetGlobalIdentifier());
		if(this->Flags.particle_species)
			WriteStream(*fs, p.GetSpecies());
		if(this->Flags.particle_parent_id && p.HasParent())
			WriteStream(*fs, p.GetParent()->GetGlobalIdentifier());
		if(this->Flags.particle_parent_species && p.HasParent()) 
			WriteStream(*fs, p.GetParent()->GetSpecies());
		if(this->Flags.particle_position) 
		{
			const auto& pos = p.GetPosition();
			*fs << scientific; 
			WriteStream(*fs, pos[0]);
			WriteStream(*fs, pos[1]);
			WriteStream(*fs, pos[2]);
			*fs << fixed;
		}
		if(this->Flags.particle_director) 
		{
			const auto& dir = p.GetDirector();
			*fs << scientific; 
			WriteStream(*fs, dir[0]);
			WriteStream(*fs, dir[1]);
			WriteStream(*fs, dir[2]);
			*fs << fixed;		
		}
		if(this->Flags.particle_neighbors)
		{
			for(auto& neighbor : p.GetNeighbors())
				WriteStream(*fs, neighbor->GetGlobalIdentifier());		
		}

		*fs << "\n";
	}

	void DLMFileObserver::Serialize(Json::Value& json) const
	{
		json["type"] = "DLMFile";
		json["frequency"] = GetFrequency();
		json["prefix"] = _prefix;
		json["colwidth"] = _w;
		json["fixedwmode"] = _fixl;
		json["delimiter"] = _dlm;
		json["extension"] = _ext;

		this->Flags.Serialize(json["flags"]);
	}
}