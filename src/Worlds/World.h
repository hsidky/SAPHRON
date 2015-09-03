#pragma once

#include "../Particles/Particle.h"
#include "../Observers/Visitable.h"
#include "../Properties/EPTuple.h"
#include "json/json.h"
#include <memory>
#include <functional>
#include <map>

namespace SAPHRON
{
	typedef std::map<int, int> CompositionList; 
	typedef std::vector<World*> WorldList;
	typedef std::vector<int> WorldIndexList;
	
	// Public interface representing the "World" in which particles live. A World object is
	// responsible for setting up the "box" and associated geometry, handling boundary conditions and
	// updating negihbor lists on particles. World implements ParticleObserver so it can "listen in" 
	// on it's particles if it needs it. Note: it is the World implementation's responsibility to register 
	// itself with particles if it needs it, and to remove this when a particle is removed.
	class World : public Visitable, public ParticleObserver
	{
	private:
		double _temperature; 
		Energy _energy;
		Pressure _pressure;

	protected:

		// Box vectors.
		double _xlength, _ylength, _zlength;

		// World optional string ID.
		std::string _stringid;

		// Visit children.
		virtual void VisitChildren(class Visitor &v) = 0;

	public:
		World(double xlength, double ylength, double zlength) : 
		_temperature(0.0), _xlength(xlength), _ylength(ylength), _zlength(zlength), 
		_stringid(""){}

		// Draw a random particle from the world.
		virtual Particle* DrawRandomParticle() = 0;

		// Draw random particles from the world and adds them to the list provided. 
		virtual void DrawRandomParticles(ParticleList& particles, unsigned int count = 1) = 0;

		// Update the neighbor list for all particles in the world.
		virtual void UpdateNeighborList() = 0;

		// Update neighbor list for a particle.
		virtual void UpdateNeighborList(Particle* particle) = 0;
		
		// Checks list of particles, and if needed updates neighbor list.
		virtual void CheckNeighborListUpdate(const ParticleList& particles) = 0;

		// Get a specific particle based on location.
		virtual Particle* SelectParticle(int location) = 0;

		// Get a specific particle based on location (const).
		virtual Particle const* SelectParticle(int location) const = 0;
		
		// Add a particle. 
		virtual void AddParticle(Particle&& particle) = 0;

		// Add a particle.
		virtual void AddParticle(Particle* particle) = 0;

		// Remove a specific particle based on location.
		virtual void RemoveParticle(int location) = 0;

		// Remove particle based on pointer.
		virtual void RemoveParticle(Particle* p) = 0;

		// Remove particle(s) based on a supplied filter.
		virtual void RemoveParticle(std::function<bool(Particle*)> filter) = 0;

		// Applies periodic boundaries to positions.
		inline void ApplyPeriodicBoundaries(Position* position) const
		{
			position->x -= _xlength*ffloor(position->x/_xlength);
			position->y -= _ylength*ffloor(position->y/_ylength);
			position->z -= _zlength*ffloor(position->z/_zlength);
		}

		// Applies minimum image convention to distances. 
		inline void ApplyMinimumImage(Position& position)
		{
			if(position.x > _xlength/2.0)
				position.x -= _xlength;
			else if(position.x < -_xlength/2.0)
				position.x += _xlength;
			
			if(position.y > _ylength/2.0)
				position.y -= _ylength;
			else if(position.y < -_ylength/2.0)
				position.y += _ylength;

			if(position.z > _zlength/2.0)
				position.z -= _zlength;
			else if(position.z < -_zlength/2.0)
				position.z += _zlength;
			/*position.x -= _xlength*anint(position.x/_xlength);
			position.y -= _ylength*anint(position.y/_ylength);
			position.z -= _zlength*anint(position.z/_zlength);*/
		}

		/***************************
		 *                         *		
		 *   Getters and setters   *
		 *                         *
		 ***************************/

		// Get number of high level particles in the world.
		virtual int GetParticleCount() const = 0;

		// Get neighbor list cutoff radius.
		virtual double GetNeighborRadius() const = 0;

		// Set neighbor list radius cutoff.
		virtual void SetNeighborRadius(double ncut) = 0;

		// Get skin thickness for neighbor list re-generation.
		virtual double GetSkinThickness() const = 0;

		// Set skin thickness for neighbor list re-generation.
		virtual void SetSkinThickness(double x) = 0;		

		// Get system composition.
		virtual const CompositionList& GetComposition() const = 0;

		// Get system volume.
		inline double GetVolume() const
		{
			return _xlength*_ylength*_zlength;
		}	

		// Get the system number density.
		double GetDensity() const
		{
			return (double)GetParticleCount()/GetVolume();
		}

		// Gets the optional string ID for a world.
		std::string GetStringID() const { return _stringid; }

		// Sets an optional string ID for a world.
		void SetStringID(std::string stringid) { _stringid = stringid; }

		// Get seed.
		virtual int GetSeed() const = 0;

		// Get world temperature.
		double GetTemperature() const { return _temperature; }

		// Sets world temperature.
		void SetTemperature(double temperature) { _temperature = temperature; }

		// Get world pressure.
		Pressure GetPressure() const { return _pressure; }
		
		// Set world pressure. 
		void SetPressure(const Pressure& p) { _pressure = p; }

		// Get world energy. 
		Energy GetEnergy() const { return _energy; }

		// Set world energy. 
		void SetEnergy(const Energy& e) { _energy = e; }

		// Get box vectors.
		inline Position GetBoxVectors() const 
		{
			return Position{_xlength, _ylength, _zlength};
		}

		// Sets the box vectors (box volume) and if scale is true, scale the coordinates of the 
		// particles in the system. Energy recalculation after this procedure is recommended.
		// If scaling is not applied, periodic boundary conditions are applied to all particles.
		// The neighbor list is auto regenerated.
		virtual void SetBoxVectors(double x, double y, double z, bool scale) = 0;

		/***********************************
		 *                                 *
		 * Other interface implementations *
		 *                                 *
		 ***********************************/

		// Accept a visitor.
		virtual void AcceptVisitor(class Visitor &v) override
		{
			v.Visit(this);
			VisitChildren(v);
		}

		// Particle observer.
		virtual void ParticleUpdate(const ParticleEvent& pEvent) override = 0;

		/**********************************
		 *                                *
		 *      Static world builders     *
		 *                                *
		 **********************************/

		// Builds a world from JSON value. Returns a pointer to world object, or throws a 
		// BuildException if validation fails. Object lifetime is caller's responsibility!
		static World* Build(const Json::Value& json);
		static World* Build(std::istream& stream);

		virtual ~World (){}
	};
}
