#pragma once 

#include "Constraint.h"
#include "../Particles/Particle.h"

namespace SAPHRON
{
	// Class for applying P2 Legendre polynomial constraint on particle 
	// director based on specified 1-d position(s). The penalty function is 
	// V = -c*P2(dot(d,n)) where d is the particle director and n is the 
	// constraint direction. Based on location in either x, y, or z plane the 
	// constraint will be applied.
	class Dynamic1DP2C : public Constraint
	{
	private:
		double _coeff;
		Director _dir;
		int _index;
		std::array<double, 2> _lim;

	public:
		// Creates a dynamic P2 constraint on a particle.
		// coeff - magnitude of bias. 
		// dir - constraint direction.
		// index - dimension (x = 0, y = 1, z = 2).
		// lim - Limits of constraint along specified dimension.
		Dynamic1DP2C(double coeff, Director dir, int index, const std::array<double, 2>& lim) : 
		_coeff(coeff), _dir(dir), _index(index), _lim(lim)
		{}

		double EvaluateEnergy(const Particle& p) const override
		{
			auto& dir = p.GetDirector();
			auto& pos = p.GetPosition();

			// Switch on index and return if not within bounds.
			switch(_index)
			{
				case 0:
					if(pos[0] < _lim[0] || pos[0] > _lim[1])
						return 0;
					break;
				case 1:
					if(pos[1] < _lim[0] || pos[1] > _lim[1])
						return 0;
					break;
				default:
					if(pos[2] < _lim[0] || pos[2] > _lim[1])
						return 0;
					break;
			}

			auto dot = fdot(dir, _dir);
			return -1.0*_coeff*(1.5*dot*dot - 0.5);
		}

		void Serialize(Json::Value& json) const override
		{
			json["coeff"] = _coeff;
			json["director"][0] = _dir[0];
			json["director"][1] = _dir[1];
			json["director"][2] = _dir[2];
			json["index"] = _index;
			json["limits"][0] = _lim[0];
			json["limits"][1] = _lim[1];
		}

	};
}