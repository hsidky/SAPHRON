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
		if((int)_forcefields.size()-1 < std::max(p1type,p2type))
			ResizeFF(std::max(p1type,p2type)+1);

		(_forcefields.at(p1type)).at(p2type) = &ff;
		(_forcefields.at(p2type)).at(p1type) = &ff;
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
		(_forcefields.at(p1type)).at(p2type) = nullptr;
		(_forcefields.at(p2type)).at(p1type) = nullptr;
	}

	// Get the number of registered forcefields.
	int ForceFieldManager::ForceFieldCount()
	{
		return factorial(_forcefields.size());
	}

	// Get forcefield by index in lower packed triangular form.
	// ex. 11, 12, 13, 22, 23, 33, etc...
	ForceField* ForceFieldManager::GetForceField(unsigned int n)
	{
		std::cout << "Forcefield requested " << n << " size: " << _forcefields.size() << std::endl;

		unsigned int k = 0;
		for(size_t i = 0; i < _forcefields.size(); ++i)
			for(size_t j = i; j < _forcefields.size(); ++j) // Assume symmetric matrix.
			{
				if(n == k++)
					return _forcefields[i][j];
			}

		return nullptr;
	}

	// Get species associated with forcefield by index.
	// If no forcefield is registered for the index or the index is out of range 
	// the pair will contain {-1, -1}.
	std::pair<int, int> ForceFieldManager::GetForceFieldTypes(unsigned int n)
	{
		unsigned int k = 0;
		for(size_t i = 0; i < _forcefields.size(); ++i)
			for(size_t j = i; j < _forcefields.size(); ++j) // Assume symmetric matrix.
			{
				if(n == k++ && _forcefields[i][j] != nullptr)
					return {i, j};
			}

		return {-1, -1};
	}
}