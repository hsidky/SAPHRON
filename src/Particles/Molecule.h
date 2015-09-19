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
		double _charge;
	protected:
		virtual void UpdateCenterOfMass() override
		{
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

	public:

		Molecule(std::string species) 
		: Particle(species), _director{0,0,0}, _checkpoint() {}

		
		// Get molecule position based on center of mass.
		inline virtual Position GetPosition() const override
		{
			return _position;			
		}

		// Get position reference.
		inline virtual const Position& GetPositionRef() const override
		{
			return _position;
		}

		// Set molecule position. 
		inline virtual void SetPosition(const Position& position) override
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
		inline virtual void SetPosition(Position&& position) override
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
		inline virtual void SetPosition(double x, double y, double z) override
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
			_director[0] = ux;
			_director[1] = uy;
			_director[2] = uz;
			this->_pEvent.director = 1;
			this->NotifyObservers();
		}

		// Gets site charge.
		inline virtual double GetCharge() const override
		{
			return _charge;
		}

		// Sets site charge
		inline virtual void SetCharge(double charge) override
		{
			this->_pEvent.SetOldCharge(_charge);
			_charge=charge;
			this->_pEvent.charge = 1;
			this->NotifyObservers();
		}

		// Get total mass of the molecule.
		inline virtual double GetMass() override
		{
			double m = 0;
			for(auto& child : *this)
				m += child->GetMass();

			return m;
		}

		virtual Particle* Clone() const override
		{
			return new Molecule(static_cast<const Molecule&>(*this));
		}
	};
}