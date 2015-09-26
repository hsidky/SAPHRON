#pragma once 

#include "Particle.h"

namespace SAPHRON
{
	class Molecule : public Particle
	{
	private:
		Director _director;
		Position _checkpoint;
		Position _position;

	public:

		Molecule(std::string species) 
		: Particle(species), _director{0 ,0, 1.0}, _checkpoint() {}

		
		// Get molecule position based on center of mass.
		virtual Position GetPosition() const override
		{
			return _position;			
		}

		// Get position reference.
		virtual const Position& GetPositionRef() const override
		{
			return _position;
		}

		// Set molecule position. 
		virtual void SetPosition(const Position& position) override
		{
			this->_pEvent.SetOldPosition(_position);
			Position dij = position - _position;
			for(auto& child : *this)
				child->SetPosition(child->GetPosition() + dij);
			_position = position;
			this->_pEvent.position = 1;
			this->NotifyObservers();
		}

		// Sets molecule position.
		virtual void SetPosition(Position&& position) override
		{
			this->_pEvent.SetOldPosition(_position);
			Position dij = position - _position;
			for(auto& child : *this)
				child->SetPosition(child->GetPosition() + dij);
			_position = position;
			this->_pEvent.position = 1;
			this->NotifyObservers();	
		}

		// Sets the molecule position.
		virtual void SetPosition(double x, double y, double z) override
		{
			this->_pEvent.SetOldPosition(_position);
			Position dij {_position[0] - x, _position[1] - y, _position[2] - z};
			for(auto& child : *this)
				child->SetPosition(child->GetPosition() + dij);
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
		virtual Director GetDirector() const override
		{
			return _director;
		}

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

		// Gets total molecule charge.
		virtual double GetCharge() const override
		{
			double c = 0;
			for(auto& child : *this)
				c += child->GetCharge();

			return c;
		}

		// Sets charge.
		virtual void SetCharge(double) override
		{
			std::cerr << "ERROR: Cannot set the charge for a molecule directly." << std::endl;
			exit(-1);
		}

		// Get total mass of the molecule.
		virtual double GetMass() const override
		{
			double m = 0;
			for(auto& child : *this)
				m += child->GetMass();

			return m;
		}

		// Sets mass.
		virtual void SetMass(double) override
		{
			std::cerr << "ERROR: Cannot set the mass for a molecule directly." << std::endl;
			exit(-1);
		}

		// Update center of mass.
		virtual void UpdateCenterOfMass() override
		{
			// We don't fire event here because it's fired by the 
			// caller.
			this->_pEvent.SetOldPosition(_position);
			this->_pEvent.position = 1;

			_position = {0, 0, 0};
			double m = 0; 
			for(auto& child : *this)
			{
				_position += child->GetPositionRef()*child->GetMass();
				m += child->GetMass();
			}	

			// Avoid divide by zero.
			if(m) _position /= m;
		}

		virtual Particle* Clone() const override
		{
			return new Molecule(static_cast<const Molecule&>(*this));
		}
	};
}