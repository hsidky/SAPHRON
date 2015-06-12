#include "ForceFieldManager.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace SAPHRON
{
	// Helper functions.
	unsigned int factorial(unsigned int n) 
	{
	    if (n == 0)
	       return 1;
	    return n * factorial(n - 1);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddForceField(std::string p1type, std::string p2type, ForceField& ff)
	{
		auto list = Particle::GetSpeciesList();
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
		_forcefields.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_forcefields.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveForceField(std::string p1type, std::string p2type)
	{
		auto list = Particle::GetSpeciesList();
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
		_forcefields.erase({p1type, p2type});
		_forcefields.erase({p2type, p1type});
	}

	// Get the number of registered forcefields.
	int ForceFieldManager::ForceFieldCount()
	{
		return _forcefields.size()/2.0; // It contains double.
	}
}