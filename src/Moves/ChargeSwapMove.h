#pragma once 

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Class for swapping the charges on two molecules
	class ChargeSwapMove : public Move
	{
		private: 
			double _prevA, _prevB;
			std::pair<Particle*, Particle*> _particlePair;
			int _rejected;
			int _performed;
			Rand _rand;
			int _seed;

		public: 
			ChargeSwapMove(int seed =2432) : 
			_prevA(0), _prevB(0), _particlePair{nullptr, nullptr},
			_rejected(0), _performed(0), _seed(seed)
			{
			}

			inline void Perform(Particle* p1, Particle* p2)
			{

				_particlePair.first = p1;
				_particlePair.second = p2;
				_prevA = p1->GetCharge();
				_prevB = p2->GetCharge();
				p1->SetCharge(_prevB);
				p2->SetCharge(_prevA);
				++_performed;			
			}

			virtual bool Perform(World&, ParticleList& particles) override
			{
				Perform(particles[0], particles[1]);
				return false;
			}

			//Draw a random particle from the world, if that particle has children, draw a random child
			inline virtual void Draw(World& world, ParticleList& particles) override
			{

				particles.resize(2);
				particles[0] = world.DrawRandomParticle();
				particles[1] = world.DrawRandomParticle();
				
				if(particles[0]->HasChildren())
				{
					auto& children = particles[0]->GetChildren();
					int n = children.size();
					particles[0]=children[_rand.int32() % n];
				}

				if(particles[1]->HasChildren())
				{
					auto& children = particles[1]->GetChildren();
					int n = children.size();
					particles[1]=children[_rand.int32() % n];
				}

				if(particles[0]->GetCharge() == particles[1]->GetCharge())
					_rejected++;				

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
				_particlePair.first->SetCharge(_prevA);
				_particlePair.second->SetCharge(_prevB);
				++_rejected;
			}

			// Get seed.
			virtual int GetSeed() override { return _seed; }

			virtual std::string GetName() override { return "ChargeSwap"; }

			// Clone move.
			Move* Clone() const override
			{
				return new ChargeSwapMove(static_cast<const ChargeSwapMove&>(*this));
			}

	};
}