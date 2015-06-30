#include "JSONObserver.h"
#include "../Ensembles/NVTEnsemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Moves/TranslateMove.h"
#include "../Worlds/World.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../ForceFields/LebwohlLasherFF.h"
#include "../ForceFields/LennardJonesFF.h"
#include "../Particles/Site.h"
#include <ctime>
#include <iomanip>

namespace SAPHRON
{
	std::string GetTimestamp()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time (&rawtime);
		timeinfo = localtime(&rawtime);

  		strftime(buffer,80,"%d-%m-%Y %r",timeinfo);
  		return std::string(buffer);
	}

	JSONObserver::JSONObserver(std::string prefix, SimFlags flags, unsigned int frequency) : 
	SimObserver(flags, frequency), _counter(0), _prefix(prefix), _root(), _cspecies(0)
	{
	}

	void JSONObserver::PreVisit()
	{
		_jsonfs = std::unique_ptr<std::ofstream>(
			new std::ofstream(_prefix + "." + std::to_string(++_counter) + ".json")
		);

		_jsonfs->precision(20);
	}

	void JSONObserver::PostVisit()
	{
		*_jsonfs 
		<< "/**********************************************\n"
		<< " *           SAPHRON JSON Snapshot            *\n"
		<< " *      Generated " << GetTimestamp() << "      *\n"
		<< " **********************************************/\n";

		Json::StreamWriterBuilder builder;
		builder["commentStyle"] = "None";
		builder["indentation"] = "   "; // or whatever you like
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(_root, &*_jsonfs);
		*_jsonfs << std::endl; // add lf and flush

		_jsonfs->close();
		_root.clear();
		_cspecies.clear();
	}

	void JSONObserver::Visit(Ensemble* ensemble)
	{	
		_root["ensemble"]["type"] = ensemble->GetName();
		_root["ensemble"]["sweeps"] = ensemble->GetIteration();
		_root["ensemble"]["kb"] = ensemble->GetBoltzmannConstant();
		_root["ensemble"]["seed"] = ensemble->GetSeed();
		_root["ensemble"]["temperature"] = ensemble->GetTemperature();
		_root["ensemble"]["pressure"] = ensemble->GetPressure().isotropic();
	}

	void JSONObserver::Visit(SAPHRON::DOSEnsemble*)
	{
	}

	void JSONObserver::Visit(MoveManager* mm)
	{
		_root["moves"]["seed"] = mm->GetSeed();

		for(auto& move : *mm)
		{
			_root["moves"][move->GetName()];
			_root["moves"][move->GetName()]["seed"] = move->GetSeed();

			if(auto* m = dynamic_cast<TranslateMove*>(move))
			{
				_root["moves"][move->GetName()]["dx"] = m->GetMaxDisplacement();
			}
		}
	}

	void JSONObserver::Visit(ForceFieldManager *ffm)
	{
		int i = 0;
		for(auto& ffmap : ffm->GetNonBondedForceFields())
		{
			const SpeciesPair& types = ffmap.first;
			ForceField* forcefield = ffmap.second;
			if(auto* ff = dynamic_cast<LebwohlLasherFF*>(forcefield))
			{
				auto& ll = _root["forcefields"][i]["LebwohlLasher"];
				ll["epsilon"] = ff->GetEpsilon();
				ll["gamma"] = ff->GetGamma();
				ll["species"][0] = Particle::GetSpeciesList()[types.first];
				ll["species"][1] = Particle::GetSpeciesList()[types.second];
			}
			else if(auto* ff = dynamic_cast<LennardJonesFF*>(forcefield))
			{
				auto& ll = _root["forcefields"][i]["LennardJones"];
				ll["epsilon"] = ff->GetEpsilon();
				ll["sigma"] = ff->GetSigma();
				ll["rcut"] = ff->GetCutoffRadius();
				ll["species"][0] = Particle::GetSpeciesList()[types.first];
				ll["species"][1] = Particle::GetSpeciesList()[types.second];
			}
		}
	}

	void JSONObserver::Visit(World* world)
	{
		_root["world"]["type"] = "Simple"; // TODO: remove hard code.
			
		Position box = world->GetBoxVectors();
		_root["world"]["size"][0] = box.x;
		_root["world"]["size"][1] = box.y;
		_root["world"]["size"][2] = box.z;
		_root["world"]["seed"] = world->GetSeed();
		_root["world"]["neighbor_list"] = world->GetNeighborRadius();
		_root["world"]["skin_thickness"] = world->GetSkinThickness();
	}

	void JSONObserver::Visit(Particle* p)
	{
		auto species = p->GetSpecies();

		// Write blueprint if it doesn't exist.
		if(std::find(_cspecies.begin(), _cspecies.end(), p->GetSpeciesID()) == _cspecies.end())
		{
			_cspecies.push_back(p->GetSpeciesID());
			_root["components"][species] = Json::Value(Json::objectValue);
			int i = 0;
			for(auto& child : p->GetChildren())
			{
				_root["components"][species]["children"][i][child->GetSpecies()]["index"] = i+1;
				++i;
			}
		}

		// If particle has children, write them to particles. If not, skip.
		if(p->HasChildren())
		{
			for(auto& child : p->GetChildren())
			{
				auto cspecies = child->GetSpecies();
				auto& pos = child->GetPositionRef();
				auto& dir = child->GetDirectorRef();
				Json::Value particle; 
				particle[cspecies]["parent"] = species;
				particle[cspecies]["position"][0] = pos.x;
				particle[cspecies]["position"][1] = pos.y;
				particle[cspecies]["position"][2] = pos.z;
				particle[cspecies]["director"][0] = dir.x;
				particle[cspecies]["director"][1] = dir.y;
				particle[cspecies]["director"][2] = dir.z;
				_root["particles"].append(particle);
			}
		}
		else
		{
			auto& pos = p->GetPositionRef();
			auto& dir = p->GetDirectorRef();
			Json::Value particle; 
			particle[species]["position"][0] = pos.x;
			particle[species]["position"][1] = pos.y;
			particle[species]["position"][2] = pos.z;
			particle[species]["director"][0] = dir.x;
			particle[species]["director"][1] = dir.y;
			particle[species]["director"][2] = dir.z;
			_root["particles"].append(particle);
		}
	}
}