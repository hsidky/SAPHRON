#pragma once 

#include "json/json.h"
#include "Worlds/World.h"
#include "ForceFields/ForceField.h"
#include "ForceFields/ForceFieldManager.h"
#include "Moves/MoveManager.h"
#include "Simulation/SimException.h"
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
		int _ltot, _msgw, _notw;

	public:
		SimBuilder() : 
		_worlds(0), _particles(0), _forcefields(0), _moves(0), _ffm(), 
		_mm(), _ltot(77), _msgw(47), _notw(_ltot - _msgw)
		{}
		
		bool BuildSimulation(std::istream& is);

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
		}
	};
}