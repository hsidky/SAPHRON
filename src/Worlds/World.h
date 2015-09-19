#pragma once

#include "../Simulation/SimInfo.h"
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
	class World : public ParticleObserver, public Visitable
	{
	private:
		double _temperature; 
		Energy _energy;
		Pressure _pressure;
		
		// World ID.
		int _id; 

		// Global world ID.
		static int _nextID;

	protected:

		// Box vectors.
		double _xlength, _ylength, _zlength;

		// World optional string ID.
		std::string _stringid;

		// Visit children.
		virtual void VisitChildren(Visitor& v) const = 0;

	public:
		World(double xlength, double ylength, double zlength) : 
		_temperature(0.0), _id(++_nextID), _xlength(xlength), 
		_ylength(ylength), _zlength(zlength) 
		{
			_stringid = "world" + std::to_string(_id);
		}

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

		// Check if neighbor lists need updating based on particle.
		virtual void CheckNeighborListUpdate(Particle* p) = 0;

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
			(*position)[0] -= _xlength*ffloor((*position)[0]/_xlength);
			(*position)[1] -= _ylength*ffloor((*position)[1]/_ylength);
			(*position)[2] -= _zlength*ffloor((*position)[2]/_zlength);
		}

		// Applies minimum image convention to distances. 
		inline void ApplyMinimumImage(Position& position)
		{
			if(position[0] > _xlength/2.0)
				position[0] -= _xlength;
			else if(position[0] < -_xlength/2.0)
				position[0] += _xlength;
			
			if(position[1] > _ylength/2.0)
				position[1] -= _ylength;
			else if(position[1] < -_ylength/2.0)
				position[1] += _ylength;

			if(position[2] > _zlength/2.0)
				position[2] -= _zlength;
			else if(position[2] < -_zlength/2.0)
				position[2] += _zlength;
			/*position[0] -= _xlength*anint(position[0]/_xlength);
			position[1] -= _ylength*anint(position[1]/_ylength);
			position[2] -= _zlength*anint(position[2]/_zlength);*/
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

		// Get the cutoff radius for forcefields.
		virtual double GetCutoffRadius() const = 0;

		// Set the cutoff radius for forcefields..
		virtual void SetCutoffRadius(double x) = 0;

		// Get the effective skin thickness of the world.
		virtual double GetSkinThickness() const = 0;

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

		// Get unique world ID.
		int GetID() const { return _id; }

		// Sets an optional string ID for a world.
		void SetStringID(std::string stringid) { _stringid = stringid; }

		// Get seed.
		virtual int GetSeed() const = 0;

		// Get world temperature.
		double GetTemperature() const { return _temperature; }

		// Sets world temperature.
		void SetTemperature(double temperature) { _temperature = temperature; }

		// Get world pressure.
		Pressure GetPressure() const 
		{
			auto p = _pressure;
			auto& sim = SimInfo::Instance();
			p.ideal = GetParticleCount()*sim.GetkB()*_temperature/GetVolume(); 
			return p; 
		}
		
		// Set world pressure. 
		void SetPressure(const Pressure& p) { _pressure = p; }

		// Increment world pressure. (aka p += dp).
		void IncrementPressure(const Pressure& dp) { _pressure += dp; }

		// Get world energy. 
		Energy GetEnergy() const { return _energy; }

		// Set world energy. 
		void SetEnergy(const Energy& e) { _energy = e; }

		// Increment world energy (e += de).
		void IncrementEnergy(const Energy& de) { _energy += de; }

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

		// Particle observer.
		virtual void ParticleUpdate(const ParticleEvent& pEvent) override = 0;

		// Accept a visitor.
		virtual void AcceptVisitor(Visitor &v) const override
		{
			VisitChildren(v);
		}

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
