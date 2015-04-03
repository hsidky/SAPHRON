#include "ForceFieldManager.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace SAPHRON
{
	// Resize Forcefield vector.
	void ForceFieldManager::ResizeFF(int n)
	{
		_forcefields.resize(n);
		for(int i = 0; i < n; i++)
			_forcefields[i].resize(n, nullptr);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddForceField(std::string p1type, std::string p2type, ForceField& ff)
	{
		auto list = Particle::GetIdentityList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);

		if(p1 == list.end() || p2 == list.end())
		{
			std::cerr << "ERROR: Uknown particle types. "
			          << "Make sure they have been registered with the identity map."
			          << std::endl;
			exit(-1);
		}

		AddForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddForceField(int p1type, int p2type, ForceField& ff)
	{
		if((int)_forcefields.size()-1 < std::max(p1type,p2type))
			ResizeFF(std::max(p1type,p2type)+1);

		(_forcefields.at(p1type)).at(p2type) = &ff;
		(_forcefields.at(p2type)).at(p1type) = &ff;
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveForceField(std::string p1type, std::string p2type)
	{
		auto list = Particle::GetIdentityList();
		auto p1 = std::find(list.begin(), list.end(), p1type);
		auto p2 = std::find(list.begin(), list.end(), p2type);
		if(p1 == list.end() || p2 == list.end())
		{
			std::cerr << "ERROR: Uknown particle types. "
			          << "Make sure they have been registered with the identity map."
			          << std::endl;
			exit(-1);
		}

		RemoveForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveForceField(int p1type, int p2type)
	{
		(_forcefields.at(p1type)).at(p2type) = nullptr;
		(_forcefields.at(p2type)).at(p1type) = nullptr;
	}
}