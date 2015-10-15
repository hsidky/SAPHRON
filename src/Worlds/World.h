#pragma once

#include "../Simulation/SimInfo.h"
#include "../Particles/Particle.h"
#include "../Observers/Visitable.h"
#include "../Properties/EPTuple.h"
#include "../Rand.h"
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
		// Cutoff radius.
		double _rcut, _rcutsq; 

		// Neighbor list cutoff radius.
		double _ncut, _ncutsq;

		// Skin thickness (calculated).
		double _skin, _skinsq;

		double _temperature; 
		Energy _energy;
		Pressure _pressure;

		// Particle list.
		ParticleList _particles;

		// Primitive particle list.
		ParticleList _primitives;

		// Random number generator.
		Rand _rand;

		// Composition of the system.
		CompositionList _composition;
		
		// Seed.
		int _seed;		

		// World ID.
		int _id; 

		// Global world ID.
		static int _nextID;

		// Visit children.
		void VisitChildren(Visitor& v) const
		{
			for(auto& particle : _particles)
				particle->AcceptVisitor(v);
		}

		void AddParticleComposition(Particle* particle);
		void RemoveParticleComposition(Particle* particle);
		void ModifyParticleComposition(const ParticleEvent& pEvent);
		void UpdateNeighborList(Particle* particle, bool clear);

		// Perform all the appropriate configuration for a new particle.
		inline void ConfigureParticle(Particle* particle)
		{
			// Add this world as an observer.
			// Propogates to children.
			particle->AddObserver(this);

			// Associate this world with particle.
			// propogates to children.
			particle->SetWorld(this);

			// Add particle to the composition list.
			AddParticleComposition(particle);

			// Update particle neighbor list. This will clear old
			// and create new for particle and children.
			this->UpdateNeighborList(particle);
		}

	protected:

		// World optional string ID.
		std::string _stringid;

	public:
		typedef ParticleList::iterator iterator;
		typedef ParticleList::const_iterator const_iterator;

		World(double rcut, int seed = 1) : 
		_rcut(rcut), _rcutsq(rcut*rcut), _ncut(0), _ncutsq(0), _skin(0), _skinsq(0), 
		_temperature(0.0), _particles(0), _primitives(0), _rand(seed), _composition(),
		_seed(seed), _id(++_nextID)
		{
			_stringid = "world" + std::to_string(_id);
			_skin = 0.30 * _rcut;
			_skinsq = _skin * _skin;
			_ncut = _rcut + _skin;
			_ncutsq = (_rcut + _skin)*(_rcut + _skin);
		}

		// Draw a random particle from the world.
		Particle* DrawRandomParticle()
		{
			size_t n = _particles.size();
			assert(n > 0);
			return _particles[_rand.int32() % n];
		}

		// Draw a random primitive from the world.
		Particle* DrawRandomPrimitive()
		{
			size_t n = _primitives.size();
			assert(n > 0);
			return _primitives[_rand.int32() % n];
		}

		// Draw random particles from the world and inserts them into particles. 
		// NOTE: this method clears the list before adding new elements.
		void DrawRandomParticles(ParticleList& particles, 
										 unsigned int count = 1) 
		{
			particles.resize(count);
			unsigned int n = this->_particles.size();
			for(unsigned int i = 0; i < count; i ++)
				particles[i] = _particles[_rand.int32() % n];
		}

		// Update the neighbor list for all particles in the world.
		void UpdateNeighborList();

		// Update neighbor list for a particle.
		void UpdateNeighborList(Particle* particle);
		
		// Checks particles and updates neighborlist if necessary.
		void CheckNeighborListUpdate(const ParticleList& particles)
		{
			for(auto& particle : particles)
			{
				auto dist = particle->GetCheckpointDist();
				ApplyMinimumImage(dist);
				if(dot(dist,dist) > _skinsq/4.0)
				{	
					UpdateNeighborList();
					return;
				}
			}
		}

		// Check if neighbor lists need updating based on particle.
		void CheckNeighborListUpdate(Particle* p)
		{
			auto dist = p->GetCheckpointDist();
			ApplyMinimumImage(dist);
			if(dot(dist,dist) > _skinsq/4.0)	
				UpdateNeighborList();
		}

		// Get a specific particle based on location.
		Particle* SelectParticle(int location)
		{
			return _particles[location];
		}

		// Get a specific particle based on location (const).
		const Particle*  SelectParticle(int location) const
		{
			return _particles[location];
		}

		// Select a primitive particle by location.
		Particle* SelectPrimitive(int location)
		{
			return _primitives[location];
		}

		// Select a primitive particle by location (const).
		const Particle* SelectPrimitive(int location) const
		{
			return _primitives[location];
		}
		
		// Add a particle. 
		void AddParticle(Particle&& particle)
		{
			_particles.push_back(std::move(&particle));
			ConfigureParticle(_particles.back()); // Do this after since we are moving.
		}

		// Add a particle.
		void AddParticle(Particle* particle)
		{
			ConfigureParticle(particle);
			_particles.push_back(particle);
		}

		// Remove a specific particle based on location.
		void RemoveParticle(int location)
		{
			Particle* p = _particles[location];
			RemoveParticle(p);
		}

		// Remove particle based on pointer.
		void RemoveParticle(Particle* particle) 
		{
			auto it = std::find(_particles.begin(), _particles.end(), particle);
			if(it != _particles.end())
			{
				particle->RemoveFromNeighbors();
				particle->ClearNeighborList();

				particle->RemoveObserver(this);
				particle->SetWorld(nullptr);
				RemoveParticleComposition(particle);

				_particles.erase(it);
			}
		}

		// Remove particle(s) based on a supplied filter.
		// TODO: NEEDS UPDATING TO CARRY OUT IMPORTANT OPERATIONS. SEE ABOVE!
		void RemoveParticle(std::function<bool(Particle*)> filter)
		{
			_particles.erase(std::remove_if(_particles.begin(), _particles.end(), filter), 
							_particles.end());
		}

		// Applies periodic boundaries to positions.
		virtual void ApplyPeriodicBoundaries(Position* position) const = 0;
	
		// Applies minimum image convention to distances. 
		virtual void ApplyMinimumImage(Position& position) const = 0;

		/***************************
		 *                         *		
		 *   Getters and setters   *
		 *                         *
		 ***************************/

		// Get number of high level particles in the world.
		int GetParticleCount() const
		{
			return (int)_particles.size();
		}

		// Get number of primitives in the world.
		int GetPrimitiveCount() const
		{
			return (int)_primitives.size();
		}

		// Get neighbor list cutoff radius.
		double GetNeighborRadius() const { return _ncut; }

		// Set neighbor list radius cutoff.
		void SetNeighborRadius(double ncut)
		{
			_ncutsq = ncut*ncut;
			_ncut = ncut;

			// Recompute skin.
			_skin = _ncut - _rcut;
			_skinsq = _skin * _skin;
		}

		// Get the cutoff radius for forcefields.
		double GetCutoffRadius() const { return _rcut; }

		// Set the cutoff radius for forcefields..
		void SetCutoffRadius(double x)
		{
			_rcut = x;
			_rcutsq = x*x;

			// Recompute skin.
			_skin = _ncut - _rcut;
			_skinsq = _skin * _skin;
		}

		// Get the effective skin thickness of the world.
		double GetSkinThickness() const { return _skin;	}

		// Get system composition.
		const CompositionList& GetComposition() const
		{
			return _composition;
		}
		// Get system volume.
		virtual double GetVolume() const = 0;

		// Get the system number density.
		double GetNumberDensity() const
		{
			return (double)GetParticleCount()/GetVolume();
		}

		// Get system density (mol/cm^3)
		double GetDensity() const 
		{
			auto& sim = SimInfo::Instance();
			return 1.0e24*(double)GetParticleCount()/(sim.GetNa()*GetVolume());
		}

		// Gets the optional string ID for a world.
		std::string GetStringID() const { return _stringid; }

		// Get unique world ID.
		int GetID() const { return _id; }

		// Sets an optional string ID for a world.
		void SetStringID(std::string stringid) { _stringid = stringid; }

		// Get seed.
		int GetSeed() const { return _seed; }

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
			p *= sim.GetPressureConv();
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

		// Sets the box vectors (box volume) and if scale is true, scale the coordinates of the 
		// particles in the system. Energy recalculation after this procedure is recommended.
		// If scaling is not applied, periodic boundary conditions are applied to all particles.
		// The neighbor list is auto regenerated.
		virtual void SetBoxVectors(double x, double y, double z, bool scale) = 0;

		// Get box vectors.
		virtual Position GetBoxVectors() const = 0;

		// Iterators.
		iterator begin() { return _particles.begin(); }
		iterator end() { return _particles.end(); }
		const_iterator begin() const { return _particles.begin(); }
		const_iterator end() const { return _particles.end(); }

		/***********************************
		 *                                 *
		 * Other interface implementations *
		 *                                 *
		 ***********************************/

		// Particle observer to update world composition.
		void ParticleUpdate(const ParticleEvent& pEvent)
		{
			if(pEvent.species)
				ModifyParticleComposition(pEvent);

			if(pEvent.child_add)
				AddParticleComposition(pEvent.GetChild());
	
			if(pEvent.child_remove)
				RemoveParticleComposition(pEvent.GetChild());
		}

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

		virtual ~World()
		{
			for(auto& p : _particles)
					delete p;

			_particles.clear();
		}
	};
}
