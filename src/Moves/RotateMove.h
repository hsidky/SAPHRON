#pragma once

#include "Move.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Class for performing particle rotations.
	class RotateMove : public Move
	{
	private:
		// Maximum angle displacement [0,2*pi].
		double _dmax;
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:
		RotateMove(double dmax, int seed = 98476) : 
		_dmax(dmax), _rand(seed), _rejected(0), _performed(0), _seed(seed)
		{
		}

		// Roate a particle about an axis (x = 1, y = 2, z = 3) 
		// "deg" degrees
		void Rotate(Particle* particle, int axis, double deg)
		{
			double phi = 0, theta = 0, psi = 0;

			// assign correct angle.
			switch(axis)
			{
				case 1:	phi = deg;
					break;
				case 2:	theta = deg;
					break;
				case 3:	psi = deg;
					break;
			}

			// Compote trig functions once.
			auto cphi = cos(phi);
			auto sphi = sin(phi);
			auto ctheta = cos(theta);
			auto stheta = sin(theta);
			auto cpsi = cos(psi);
			auto spsi = sin(psi);

			// Build rotation matrix
			arma::mat33 R {{ ctheta*cpsi, cphi*spsi+sphi*stheta*cpsi, sphi*spsi-cphi*stheta*cpsi},
						   {-ctheta*spsi, cphi*cpsi-sphi*stheta*spsi, sphi*cpsi+cphi*stheta*spsi},
						   {      stheta,               -sphi*ctheta,                cphi*ctheta}};

			// First rotate particle director.
			particle->SetDirector(R*particle->GetDirectorRef());

			// Get COM.
			auto& com = particle->GetPositionRef();

			// Rotate particle children if it has any 
			for(auto& child : *particle)
			{
				child->SetPosition(R*(child->GetPositionRef()-com) + com);
				child->SetDirector(R*child->GetDirectorRef());
			}
		}

		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{

		}

		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{

		}

		double GetMaxAngle() const { return _dmax; }

		virtual double GetAcceptanceRatio() const override
		{
			return 1.0-(double)_rejected/_performed;
		};

		virtual void ResetAcceptanceRatio() override
		{
			_performed = 0;
			_rejected = 0;
		}

		// Get seed.
		virtual int GetSeed() const override { return _seed; }

		virtual std::string GetName() const override { return "Rotate"; }

		// Clone move.
		Move* Clone() const override
		{
			return new RotateMove(static_cast<const RotateMove&>(*this));
		}

	};
}