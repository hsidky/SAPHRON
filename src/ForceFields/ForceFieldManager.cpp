#include "ForceFieldManager.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace SAPHRON
{
	// Adds a forcefield to the manager.
	void ForceFieldManager::AddNonBondedForceField(std::string p1type, std::string p2type, ForceField& ff)
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

		AddNonBondedForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddNonBondedForceField(int p1type, int p2type, ForceField& ff)
	{
		_nonbondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_nonbondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniquenbffs.find({p1type, p2type}) == _uniquenbffs.end() && 
			_uniquenbffs.find({p2type, p1type}) == _uniquenbffs.end())
			_uniquenbffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveNonBondedForceField(std::string p1type, std::string p2type)
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

		RemoveNonBondedForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveNonBondedForceField(int p1type, int p2type)
	{
		_nonbondedforcefields.erase({p1type, p2type});
		_nonbondedforcefields.erase({p2type, p1type});

		_uniquenbffs.erase({p1type, p2type});
		_uniquenbffs.erase({p2type, p1type});

	}

	// Get the number of registered forcefields.
	int ForceFieldManager::NonBondedForceFieldCount()
	{
		return _uniquenbffs.size();
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddBondedForceField(std::string p1type, std::string p2type, ForceField& ff)
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

		AddBondedForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddBondedForceField(int p1type, int p2type, ForceField& ff)
	{
		_bondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_bondedforcefields.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniquebffs.find({p1type, p2type}) == _uniquebffs.end() && 
			_uniquebffs.find({p2type, p1type}) == _uniquebffs.end())
			_uniquebffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveBondedForceField(std::string p1type, std::string p2type)
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

		RemoveBondedForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveBondedForceField(int p1type, int p2type)
	{
		_bondedforcefields.erase({p1type, p2type});
		_bondedforcefields.erase({p2type, p1type});

		_uniquebffs.erase({p1type, p2type});
		_uniquebffs.erase({p2type, p1type});

	}

	// Get the number of registered forcefields.
	int ForceFieldManager::BondedForceFieldCount()
	{
		return _uniquebffs.size();
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddElectrostaticForceField(std::string p1type, std::string p2type, ForceField& ff)
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

		AddElectrostaticForceField(p1-list.begin(), p2-list.begin(), ff);
	}

	// Adds a forcefield to the manager.
	void ForceFieldManager::AddElectrostaticForceField(int p1type, int p2type, ForceField& ff)
	{
		_electrostaticforcefield.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
		_electrostaticforcefield.insert(std::pair<SpeciesPair, ForceField*>({p2type, p1type}, &ff));

		// Must be unique.
		if(_uniquebffs.find({p1type, p2type}) == _uniqueeffs.end() && 
			_uniquebffs.find({p2type, p1type}) == _uniqueeffs.end())
			_uniquebffs.insert(std::pair<SpeciesPair, ForceField*>({p1type, p2type}, &ff));
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveElectrostaticForceField(std::string p1type, std::string p2type)
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

		RemoveElectrostaticForceField(p1-list.begin(), p2-list.begin());
	}

	// Removes a forcefield from the manager.
	void ForceFieldManager::RemoveElectrostaticForceField(int p1type, int p2type)
	{
		_electrostaticforcefield.erase({p1type, p2type});
		_electrostaticforcefield.erase({p2type, p1type});

		_uniqueeffs.erase({p1type, p2type});
		_uniqueeffs.erase({p2type, p1type});

	}

	// Get the number of registered forcefields.
	int ForceFieldManager::ElectrostaticForceFieldCount()
	{
		return _uniqueeffs.size();
	}

}