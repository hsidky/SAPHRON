#pragma once 

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Class for translating a random particle a maximum of "dx" distance.
	class ImplicitProtonationMove : public Move
	{
		private: 
			double _prevcharge;
			double _dcharge;
			Rand _rand;
			Particle* _particle;
			int _rejected;
			int _performed;
			int _seed;

		public: 
			ImplicitProtonationMove(double dcharge, int seed = 2496) : 
			_prevcharge(0), _dcharge(dcharge), _rand(seed), _particle(nullptr), 
			_rejected(0), _performed(0), _seed(seed)
			{
			}

			// This is for unit testing convenience. It is not part of the interface.
			inline void Perform(Particle* particle)
			{
				_particle = particle;
				_prevcharge = particle->GetCharge();
				_particle->SetCharge(_prevcharge+_dcharge);

				++_performed;			
			}

			//Draw a random particle from the world, if that particle has children, draw a random child
			inline virtual void Draw(World& world, ParticleList& particles) override
			{

				particles.resize(1);
				particles[0] = world.DrawRandomParticle();
				
				if(particles[0]->HasChildren())
				{
					auto& children = particles[0]->GetChildren();
					int n = children.size();
					particles[0]=children[_rand.int32() % n];
				}


			}

			//(De)protenate a particle based on if it is charged or not.
			//TODO: current method only works for monoprotic bases and acids
			inline virtual bool Perform(World&, ParticleList& particles) override
			{
				_particle = particles[0];
				_prevcharge = _particle->GetCharge();
				
				if(_prevcharge)
					_particle->SetCharge(0);
				else
					_particle->SetCharge(_dcharge);

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
				_particle->SetCharge(_prevcharge);
				++_rejected;
			}

			// Get seed.
			virtual int GetSeed() override { return _seed; }

			virtual std::string GetName() override { return "Translate"; }

			// Clone move.
			Move* Clone() const override
			{
				return new ImplicitProtonationMove(static_cast<const ImplicitProtonationMove&>(*this));
			}

	};
}