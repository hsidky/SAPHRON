#pragma once 

#include "../Rand.h"
#include "Move.h"

namespace SAPHRON
{
	// Class for translating a random particle a maximum of "dx" distance.
	class TranslateMove : public Move
	{
		private: 
			Position _prevPos;
			double _dx;
			Rand _rand;
			Particle* _particle;
			int _rejected;
			int _performed;

		public: 
			TranslateMove(double dx, int seed = 2496) : 
			_prevPos({0.0, 0.0, 0.0}), _dx(dx), _rand(seed), _rejected(0), _performed(0)
			{
			}

			// This is for unit testing convenience. It is not part of the interface.
			inline void Perform(Particle* particle)
			{
				_particle = particle;
				_prevPos = particle->GetPositionRef();
				_particle->SetPosition(_prevPos.x + _dx*(_rand.doub()-0.5), 
									   _prevPos.y + _dx*(_rand.doub()-0.5), 
									   _prevPos.z + _dx*(_rand.doub()-0.5));
				++_performed;			
			}

			virtual bool Perform(World& world, ParticleList& particles) override
			{
				_particle = particles[0];
				_prevPos = _particle->GetPositionRef();
				
				Position newPos({_prevPos.x + _dx*(_rand.doub()-0.5), 
								 _prevPos.y + _dx*(_rand.doub()-0.5), 
								 _prevPos.z + _dx*(_rand.doub()-0.5)});
				
				world.ApplyPeriodicBoundaries(newPos);
				_particle->SetPosition(newPos);
				++_performed;						
				
				return false;
			}

			virtual void Draw(World& world, ParticleList& particles) override
			{
				particles.resize(1);
				particles[0] = world.DrawRandomParticle();
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
				_particle->SetPosition(_prevPos);
				++_rejected;
			}

			virtual std::string GetName() override { return "Translate"; }

			// Clone move.
			Move* Clone() const override
			{
				return new TranslateMove(static_cast<const TranslateMove&>(*this));
			}

	};
}