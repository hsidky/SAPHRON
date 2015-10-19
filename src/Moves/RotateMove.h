#pragma once

#include "Move.h"
#include "../Utils/Rand.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../DensityOfStates/DOSOrderParameter.h"

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

		// Rotate a particle and children given a rotation matrix R.
		void Rotate(Particle* particle, const Matrix3D& R)
		{
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

		// Create a rotation matrix which will rotate a vector 
		// abount an axis (x = 1, y = 2, z = 3) "deg" degrees.
		Matrix3D GenRotationMatrix(int axis, double deg)
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

			// Build rotation matrix. 
			// TODO: CHECK RVO.
			return {{ ctheta*cpsi, cphi*spsi+sphi*stheta*cpsi, sphi*spsi-cphi*stheta*cpsi},
					{-ctheta*spsi, cphi*cpsi-sphi*stheta*spsi, sphi*cpsi+cphi*stheta*spsi},
					{      stheta,               -sphi*ctheta,                cphi*ctheta}};
		}	

		// Roate a particle about an axis (x = 1, y = 2, z = 3) 
		// "deg" degrees
		void Rotate(Particle* particle, int axis, double deg)
		{
			Matrix3D R = GenRotationMatrix(axis, deg);
			Rotate(particle, R);
		}

		// Perform rotation on a random particle from a random world.
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) override
		{
			// Get random particle from random world.
			World* w = wm->GetRandomWorld();
			Particle* particle = w->DrawRandomParticle();

			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());

			// Choose random axis, and generate random angle.
			int axis = _rand.int32() % 3 + 1;
			double deg = (2.0*_rand.doub() - 1.0)*_dmax;
			Matrix3D R = GenRotationMatrix(axis, deg);

			// Rotate particle.
			Rotate(particle, R);
			++_performed;

			// Evaluate final particle energy and get delta E. 
			auto ef = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());
			Energy de = ef.energy - ei.energy;

			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle->GetChildren());

			// Get sim info for kB.
			auto sim = SimInfo::Instance();

			// Acceptance probability.
			double p = exp(-de.total()/(w->GetTemperature()*sim.GetkB()));
			p = p > 1.0 ? 1.0 : p;

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				// Rotate back. 
				Rotate(particle, R.t());
				++_rejected;
			}
			else
			{
				// Update energies and pressures.
				w->IncrementEnergy(de);
				w->IncrementPressure(ef.pressure - ei.pressure);
			}
		}

		// DOS interface for move.
		virtual void Perform(World* w, ForceFieldManager* ffm, DOSOrderParameter* op , const MoveOverride& override) override
		{
			Particle* particle = w->DrawRandomParticle();

			// Evaluate initial particle energy. 
			auto ei = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());
			auto opi = op->EvaluateOrderParameter(*w);

			// Choose random axis, and generate random angle.
			int axis = _rand.int32() % 3 + 1;
			double deg = (2.0*_rand.doub() - 1.0)*_dmax;
			Matrix3D R = GenRotationMatrix(axis, deg);

			// Rotate particle.
			Rotate(particle, R);
			++_performed;

			// Evaluate final particle energy and get delta E. 
			auto ef = ffm->EvaluateHamiltonian(*particle, w->GetComposition(), w->GetVolume());
			Energy de = ef.energy - ei.energy;

			// Update energies and pressures.
			w->IncrementEnergy(de);
			w->IncrementPressure(ef.pressure - ei.pressure);

			auto opf = op->EvaluateOrderParameter(*w);
			
			// Update neighbor list if needed.
			w->CheckNeighborListUpdate(particle);

			// Acceptance probability.
			double p = op->AcceptanceProbability(ei.energy, ef.energy, opi, opf, *w);

			// Reject or accept move.
			if(!(override == ForceAccept) && (p < _rand.doub() || override == ForceReject))
			{
				Rotate(particle, R.t());
				
				// Update energies and pressures.
				w->IncrementEnergy(-1.0*de);
				w->IncrementPressure(ei.pressure - ef.pressure);
				
				++_rejected;
			}	

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