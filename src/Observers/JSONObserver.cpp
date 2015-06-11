#include "JSONObserver.h"
#include "../Ensembles/NVTEnsemble.h"
#include "../Ensembles/DOSEnsemble.h"
#include "../Moves/TranslateMove.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	JSONObserver::JSONObserver(std::string prefix, SimFlags flags, unsigned int frequency) : 
	SimObserver(flags, frequency), _counter(0), _prefix(prefix)
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
		*_jsonfs << _root << std::endl;
		_jsonfs->close();
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

	void JSONObserver::Visit(SAPHRON::DOSEnsemble *e)
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

	void JSONObserver::Visit(Particle*)
	{

	}
}