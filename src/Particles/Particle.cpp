#include "Particle.h"

namespace SAPHRON
{
	// Set the identity of a particle.
	void Particle::SetIdentity(std::string identifier)
	{
		auto search = std::find(_identityList.begin(), _identityList.end(), identifier);
		if (search != _identityList.end())
			_ID = search - _identityList.begin();
		else
		{
			_ID = (int) _identityList.size();
			_identityList.push_back(identifier);
		}

		_identifier = identifier;
	}

	// Set the identity of the particle.
	void Particle::SetIdentity(int id)
	{
		assert(id < (int)_identityList.size());
		_ID = id;
		_identifier = _identityList[id];
	}

	IdentityList Particle::_identityList;
	int Particle::_nextID = 0;
}
