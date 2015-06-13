#pragma once 

#include "GibbsMove.h"
#include "../Rand.h"

namespace SAPHRON
{
	// Class for performing a particle swap between two worlds. It removes a particle from 
	// a random world and inserts it randomly into another in a random location.
	// Note: this does not implement the Move interface (leaves them empty). 
	// It only implements the GibbsMove interface.
	class ParticleSwapMove : public GibbsMove
	{
	private:
		Particle* _particle;
		World* _prevWorld;
		Position _prevPos;
		NeighborList _prevList;
		Rand _rand;
		int _rejected;
		int _performed;
		int _seed;

	public:
		ParticleSwapMove(int seed = 3429329) : 
		_particle(nullptr), _prevWorld(nullptr), _prevPos(), _prevList(0), _rand(seed), 
		_rejected(0), _performed(0), _seed(seed)
		{
			_prevList.reserve(100);
		}

		// Unimplemented for single world.
		virtual void Draw(World&, ParticleList&) override {}

		// Unimplemented for single world.
		virtual bool Perform(World&, ParticleList&) override { return false; }

		// Draws a particle from a random world.
		virtual void Draw(const WorldList& worlds, ParticleList& particles) override
		{
			if(Particle* particle = worlds[_rand.int32() % worlds.size()]->DrawRandomParticle())
			{
				particles.resize(1);
				particles[0] = particle;
			}
			else
				particles.resize(0);
		}

		// Swaps a particle from its world to another random world.
		virtual bool Perform(const WorldList& worlds, ParticleList& particles) override
		{
			// There must be more than one world in the list.
			assert(worlds.size() != 1);

			if(!particles.size())
				return false;

			_particle = particles[0];
			_prevWorld = _particle->GetWorld();

			// Backup particle's neighbor list and position.
			_prevList = _particle->GetNeighbors();
			_prevPos = _particle->GetPositionRef();

			// Remove particle from its world.
			_prevWorld->RemoveParticle(_particle);
			_particle->ClearNeighborList();

			// Insert into new random world.
			World* newworld = worlds[_rand.int32() % worlds.size()];
				
			while(newworld == _prevWorld)
				newworld = worlds[_rand.int32() % worlds.size()];

			// Generate random new coordinates for new particle.
			Position pos = newworld->GetBoxVectors();
			pos.x *= _rand.doub();
			pos.y *= _rand.doub();
			pos.z *= _rand.doub();

			_particle->SetPosition(pos);
			newworld->AddParticle(_particle);
			newworld->UpdateNeighborList(_particle);
			++_performed;

			return false;
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
			World* newworld = _particle->GetWorld();
			newworld->RemoveParticle(_particle);
			auto& neighbors = _particle->GetNeighbors();
			neighbors = _prevList;
			_particle->SetPosition(_prevPos);
			_prevWorld->AddParticle(_particle);
			++_rejected;
		}

		// Get seed.
		virtual int GetSeed() override { return _seed; }

		virtual std::string GetName() override { return "ParticleSwap"; }

		// Clone move.
		Move* Clone() const override
		{
			return new ParticleSwapMove(static_cast<const ParticleSwapMove&>(*this));
		}

	};
}