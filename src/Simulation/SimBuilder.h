#pragma once 

#include "json/json.h"
#include "Worlds/World.h"
#include "Worlds/WorldManager.h"
#include "ForceFields/ForceField.h"
#include "ForceFields/ForceFieldManager.h"
#include "Moves/MoveManager.h"
#include "Simulation/SimException.h"
#include "SimObserver.h"
#include "../Utils/Histogram.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "Simulation.h"
#include <iostream>
#include <iomanip>

namespace SAPHRON
{
	// Class for creating, managing and running a simulation. 
	class SimBuilder
	{
	private: 
		WorldList _worlds;
		ParticleList _particles;
		FFList _forcefields;
		MoveList _moves;
		ForceFieldManager _ffm;
		MoveManager _mm;
		WorldManager _wm;
		ObserverList _observers;
		Histogram* _hist;
		DOSOrderParameter* _orderp;
		Simulation* _sim;
		int _ltot, _msgw, _notw;

	public:
		SimBuilder() : 
		_worlds(0), _particles(0), _forcefields(0), _moves(0), _ffm(), 
		_mm(), _wm(), _observers(0), _hist(nullptr), _orderp(nullptr), 
		_sim(nullptr), _ltot(81), _msgw(51), _notw(_ltot - _msgw)
		{}
		
		bool BuildSimulation(const std::string& filename);
		Simulation* GetSimulation() { return _sim; }
		
		~SimBuilder()
		{
			for(auto& world : _worlds)
				delete world;
			_worlds.clear();

			for(auto& p : _particles)
				delete p;
			_particles.clear();

			for(auto& f: _forcefields)
				delete f;
			_forcefields.clear();

			for(auto& m : _moves)
				delete m;
			_moves.clear();

			for(auto& o : _observers)
				delete o;
			_observers.clear();

			delete _hist;
			delete _orderp;
			delete _sim;
		}
	};
}