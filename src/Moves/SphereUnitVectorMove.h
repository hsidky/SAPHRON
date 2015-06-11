#pragma once

#include "../Rand.h"
#include "Move.h"
#include <cmath>

namespace SAPHRON
{
	// Class for selecting a random unit vector on a sphere for a particle director.
	class SphereUnitVectorMove : public Move
	{
		private:
			Director _prevD;
			Rand _rand;
			Particle* _particle;
			int _rejected;
			int _performed;
			int _seed;

		public:
			SphereUnitVectorMove (int seed = 3) : _prevD({0.0, 0.0, 0.0}), _rand(seed),
			 _rejected(0), _performed(0), _seed(seed) {}

			// Select a new random unit vector on a sphere.
			inline void Perform(Particle* particle)
			{
				_particle = particle;
				// Record prev director.
				_prevD = particle->GetDirectorRef();

				// Get new unit vector.
				double v3 = 0;
				do
				{
					double v1 = _rand.doub();
					double v2 = _rand.doub();
					v1 = 1 - 2 * v1;
					v2 = 1 - 2 * v2;
					v3 = v1*v1 + v2*v2;
					if(v3 < 1)
						particle->SetDirector(2.0*v1*sqrt(1 - v3), 2.0*v2*sqrt(1 - v3), 1.0-2.0*v3);
				} while(v3 > 1);

				++_performed;
			}

			virtual bool Perform(World&, ParticleList& particles) override
			{
				Perform(particles[0]);
				return false;
			}

			virtual void Draw(World& world, ParticleList& particles) override
			{
				particles.resize(1);
				particles[0] = world.DrawRandomParticle();;
			}

			virtual double GetAcceptanceRatio() override
			{
				return 1.0-(double)_rejected/_performed;
			};

			virtual void ResetAcceptanceRatio() override
			{
				_performed = 0;
				_rejected = 0;
			}

			// Undo move.
			virtual void Undo() override
			{
				_particle->SetDirector(_prevD);
				++_rejected;
			}

			// Get seed.
			virtual int GetSeed() override { return _seed; }

			virtual std::string GetName() override { return "SphereUV";	}

			// Clone move.
			Move* Clone() const override
			{
				return new SphereUnitVectorMove(static_cast<const SphereUnitVectorMove&>(*this));
			}
	};
}
