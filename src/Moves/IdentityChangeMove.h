#pragma once

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Identity change move. This move merely changes the identity of a particle. Meaning that it is useful
	// when performing (semi)grand canonical simulations on spins or atoms, but NOT on moleucles or
	// composite particle types. The only thing altered is the species and string.
	class IdentityChangeMove : public Move
	{
		private:
			Particle* _particle;
			int _prevSpecies;
			Rand _rand;
			int _speciesCount;
			int _rejected;
			int _performed;
			int _seed;

		public:
			IdentityChangeMove(int speciesCount, int seed = 1337) :
				_particle(nullptr), _prevSpecies(0), _rand(seed), _speciesCount(speciesCount),
				_rejected(0), _performed(0), _seed(seed) {}

			// Reassigns the species of a site with a new random one.
			inline void Perform(Particle* particle)
			{
				_particle = particle;
				// Record previous species
				_prevSpecies = particle->GetSpeciesID();

				particle->SetSpecies(_rand.int32() % _speciesCount);
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

			// Get seed.
			virtual int GetSeed() override { return _seed; }

			// Undo the move on a site.
			void Undo() override
			{
				_particle->SetSpecies(_prevSpecies);
				++_rejected;
			}

			virtual std::string GetName() override { return "IdentityChange"; }

			// Clone move.
			virtual Move* Clone() const override
			{
				return new IdentityChangeMove(
				               static_cast<const IdentityChangeMove&>(*this)
				               );
			}
	};
}
