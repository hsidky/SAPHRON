#pragma once 

#include "json/json.h"
#include "Worlds/World.h"
#include "ForceFields/ForceField.h"
#include "ForceFields/ForceFieldManager.h"
#include "Simulation/SimException.h"
#include <iostream>
#include <iomanip>


namespace SAPHRON
{
	// Class for creating, managing and running a simulation. 
	class NewSimBuilder
	{
	private: 
		WorldList _worlds;
		ParticleList _particles;
		FFList _forcefields;
		ForceFieldManager _ffm;
		int _ltot, _msgw, _notw;

	public:
		NewSimBuilder() : 
		_worlds(0), _particles(0), _forcefields(0), _ffm(), 
		_ltot(77), _msgw(47), _notw(_ltot - _msgw)
		{}
		
		bool BuildSimulation(std::istream& is);

		~NewSimBuilder()
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
		}
	};
}