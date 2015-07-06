#pragma once 
#include "Position.h"
#include "Director.h"

namespace SAPHRON
{
	// Forward declare. 
	class Particle;
	
	// Particle event class.
	class ParticleEvent
	{
		private:
			Particle* _particle;
			Director _oldDirector;
			Position _oldPosition;
			int _oldSpecies;
			double _oldCharge;

		public:
			ParticleEvent(Particle* particle) : 
				_particle(particle), _oldDirector(), _oldPosition(), 
				_oldSpecies(0), _oldCharge(), mask(0) {}

			union
			{
				struct
				{
					unsigned int position : 1;
					unsigned int director : 1;
					unsigned int species : 1;
					unsigned int neighbors : 1;
					unsigned int charge : 1;
				};
				bool mask;
			};

			inline void SetOldDirector(const Director& director)
			{
				_oldDirector = director;
			}

			inline const Director& GetOldDirector() const
			{
				return _oldDirector;
			}

			inline void SetOldPosition(const Position& position)
			{
				_oldPosition = position;
			}

			inline const Position& GetOldPosition() const
			{
				return _oldPosition;
			}

			inline void SetOldCharge(const double& charge)
			{
				_oldCharge = charge;
			}

			inline double GetOldCharge() const
			{
				return _oldCharge;
			}

			inline void SetOldSpecies(const int& species)
			{
				_oldSpecies = species;
			}

			inline int GetOldSpecies() const
			{
				return _oldSpecies;
			}

			inline Particle* GetParticle() const
			{
				return _particle;
			}
	};
}