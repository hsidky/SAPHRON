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
			double _charge;
			double _mass;

		public:

			// Initializes a site at a given position with a given director and species.
			Site(Position position, Director director, std::string species) : 
			Particle(species), _position(position), _director(director), _checkpoint(),
			_charge(0.0), _mass(1.0) 
			{}

			// Gets site position.
			virtual Position GetPosition() const override {	return _position; }

			// Get position reference.
			virtual const Position& GetPositionRef() const override
			{
				return _position;
			}

			// Sets site position.
			virtual void SetPosition(const Position& position) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position = position;
				this->_pEvent.position = 1; 
				this->NotifyObservers();
			}

			// Sets site position.
			virtual void SetPosition(Position&& position) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position = position;
				this->_pEvent.position = 1;
				this->NotifyObservers();
			}

			// Sets site position
			virtual void SetPosition(double x, double y, double z) override
			{
				this->_pEvent.SetOldPosition(_position);
				_position[0] = x;
				_position[1] = y;
				_position[2] = z;
				this->_pEvent.position = 1;
				this->NotifyObservers();
			}

			// Set position checkpoint.
			virtual void SetCheckpoint() override
			{
				_checkpoint = _position;
			}

			// Get position checkpoint.
			virtual const Position& GetCheckpoint() const override
			{
				return _checkpoint;
			}

			// Get distance from checkpoint.
			virtual Position GetCheckpointDist() const override
			{
				return _position - _checkpoint;
			}

			// Gets site director.
			virtual Director GetDirector() const override {	return _director; }

			// Get director reference.
			virtual const Director& GetDirectorRef() const override	
			{ 
				return _director;	
			}

			// Sets site director.
			virtual void SetDirector(const Director& director) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director = director;
				this->_pEvent.director = 1;
				this->NotifyObservers();
			}

			// Sets site director.
			virtual void SetDirector(Director&& director) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director = director;
				this->_pEvent.director = 1;
				this->NotifyObservers();
			}
			
			// Sets site director.
			virtual void SetDirector(double ux, double uy, double uz) override
			{
				this->_pEvent.SetOldDirector(_director);
				_director[0] = ux;
				_director[1] = uy;
				_director[2] = uz;
				this->_pEvent.director = 1;
				this->NotifyObservers();
 			}

 			// Gets site charge.
			virtual double GetCharge() const override {	return _charge;	}

			// Sets site charge
			virtual void SetCharge(double charge) override
			{
				this->_pEvent.SetOldCharge(_charge);
				_charge=charge;
				this->_pEvent.charge = 1;
				this->NotifyObservers();
			}

			// Get site mass.
 			virtual double GetMass() const override	{ return _mass;	}

 			// Set site mass.
 			virtual void SetMass(double m) override	{ _mass = m; }

			virtual Particle* Clone() const override
			{
				return new Site(static_cast<const Site&>(*this));
			}
	};
}
