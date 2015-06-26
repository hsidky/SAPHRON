#pragma once

#include "Particle.h"

namespace SAPHRON
{
	// The most basic particle. This represents a site on a lattice.
	class Site : public Particle
	{
		private:
			Position _position;
			Director _director;
			Position _checkpoint;

		public:

			// Initializes a site at a given position with a given director and species.
			Site(Position position, Director director, std::string species)
				: Particle(species), _position(position), _director(director), _checkpoint(){}

			// Gets site position.
			inline virtual Position GetPosition() const override
			{
				return _position;
			}

			// Get position reference.
			inline virtual const Position& GetPositionRef() const override
			{
				return _position;
			}

			// Sets site position.
			inline virtual void SetPosition(const Position& position) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position = position;
				this->_pEvent.position = 1; 
				this->NotifyObservers();
			}

			// Sets site position.
			inline virtual void SetPosition(Position&& position) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position = position;
				this->_pEvent.position = 1;
				this->NotifyObservers();
			}

			// Sets site position
			inline virtual void SetPosition(double x, double y, double z) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position.x = x;
				_position.y = y;
				_position.z = z;
				this->_pEvent.position = 1;
				this->NotifyObservers();
			}

			// Set position checkpoint.
			inline virtual void SetCheckpoint() override
			{
				_checkpoint = _position;
			}

			// Get position checkpoint.
			inline virtual const Position& GetCheckpoint() const override
			{
				return _checkpoint;
			}

			// Get distance from checkpoint.
			inline virtual Position GetCheckpointDist() const override
			{
				return _position - _checkpoint;
			}

			// Gets site director.
			inline virtual Director GetDirector() const override
			{
				return _director;
			}

			// Get director reference.
			inline virtual const Director& GetDirectorRef() const override
			{
				return _director;
			}

			// Sets site director.
			inline virtual void SetDirector(const Director& director) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director = director;
				this->_pEvent.director = 1;
				this->NotifyObservers();
			}

			// Sets site director.
			inline virtual void SetDirector(Director&& director) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director = director;
				this->_pEvent.director = 1;
				this->NotifyObservers();
			}
			
			// Sets site director.
			inline virtual void SetDirector(double ux, double uy, double uz) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director.x = ux;
				_director.y = uy;
				_director.z = uz;
				this->_pEvent.director = 1;
				this->NotifyObservers();
 			}

 			inline virtual double GetMass() override
 			{
 				return 1.0;
 			}

			virtual Particle* Clone() const override
			{
				return new Site(static_cast<const Site&>(*this));
			}
	};
}
