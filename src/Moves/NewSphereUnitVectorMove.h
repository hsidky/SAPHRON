#pragma once

#include "../Rand.h"
#include "NewMove.h"
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

		public:
			SphereUnitVectorMove (int seed = 3) : _prevD({0.0, 0.0, 0.0}), _rand(seed) {}

			// Select a new random unit vector on a sphere.
			virtual void Perform(Particle& particle) override
			{
				_particle = &particle;

				// Record prev director.
				_prevD = _particle->GetDirector();

				// Get new unit vector.
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
						_particle->SetDirector({2*v1*sqrt(1 - v3), 2*v2*sqrt(1 - v3), 1-2*v3});
				} while(v3 > 1);
			}

			// Undo move.
			virtual void Undo() override
			{
				_particle->SetDirector(_prevD);
			}

			// Clone move.
			Move* Clone() const override
			{
				return new SphereUnitVectorMove(static_cast<const SphereUnitVectorMove&>(*this));
			}
	};
}
