#pragma once 

#include "../Particles/NewParticle.h"
#include "../Utils/Rand.h"
#include <Eigen/Dense>

namespace SAPHRON
{
	using Matrix3 = Eigen::Matrix3d;
	using CompositionList = std::vector<uint>;
	class NewWorld
	{
	private:
		// Neighbor cutoff radius.
		double ncut_, ncutsq_;

		// Box matrix. 
		Matrix3 H_, Hinv_;

		// Periodic boundaries. 
		bool periodx_, periody_, periodz_;

		// System properties.
		double temperature_;

		// Particle list. 
		std::vector<NewParticle> particles_;

		// Site list.
		std::vector<Site> sites_;

		/////////////////////////////
		//  Cell list properties   //
		/////////////////////////////

		// Ratio of ncut to define cell size.
		// cellsize = cellratio*ncut. 
		double cellratio_;

		// Cell list mask pointer.
		std::vector<uint> pmask_;

		// Cell and cell pointer.
		std::vector<uint> cell_, cellptr_;

		// Random number generator.
		Rand rand_;

		// Composition of particles and sites system. 
		CompositionList sitecomp_, particlecomp_;

		// Seed.
		uint seed_;

		// World ID.
		uint id_;

		// Global world ID counter.
		static int nextID_;

		// Adds composition of particle and sites to local map.
		void AddParticleComposition(const NewParticle& p)
		{
			int id = p.GetSpecies();
			if((int)particlecomp_.size() - 1 < id)
				particlecomp_.resize(id + 1, 0);

			++particlecomp_[id];

			// Add sites.
			for(uint i = 0; i < p.SiteCount(); ++i)
			{
				int sid = p.GetSpecies(i);
				if((int)sitecomp_.size() - 1 < sid)
					sitecomp_.resize(sid + 1, 0);

				++sitecomp_[sid];
			}
		}

		// Removes composition of particle and sites from local map.
		void RemoveParticleComposition(const NewParticle& p)
		{
			int id = p.GetSpecies();
			--particlecomp_[id];

			for(uint i = 0; i < p.SiteCount(); ++i)
			{
				int sid = p.GetSpecies(i);
				--sitecomp_[sid];
			}
		}

		// Private copy constructor. Copying a world is pretty involved.
		// We'll deal with this later.
		NewWorld(const NewWorld&);

	public: 
		NewWorld(double x, double y, double z, double ncut, uint seed) : 
		ncut_(ncut), ncutsq_(ncut*ncut), H_(Eigen::Matrix3d::Zero()), 
		Hinv_(Eigen::Matrix3d::Zero()), periodx_(true),
		periody_(true), periodz_(true), temperature_(0.), particles_(),
		sites_(0), cellratio_(0.2), pmask_(0), cell_(0), cellptr_(0), 
		rand_(seed), sitecomp_(0), particlecomp_(0), seed_(seed), 
		id_(nextID_++)
		{
			H_(0,0) = x;
			H_(1,1) = y;
			H_(2,2) = z;
			Hinv_ = H_.inverse();

			// TODO: benchmark reserve capacity, include particles and sites.
		}

		// Draw a random particle from the world. 
		// Returns nullptr if nothing is drawn.
		NewParticle* DrawRandomParticle()
		{
			auto n = particles_.size();
			if(n < 1) return nullptr;
			return &particles_[rand_.int32() % n];
		}

		// Uniformly draws a particle from the world based on a list of species. 
		// This selects a random particle weighted by composition.
		NewParticle* DrawParticleFromSpeciesList(const std::vector<uint>& species)
		{
			// Compute total number of drawable species.
			auto tot = 0;
			for(auto& s : species)
				tot += particlecomp_[s];

			// Pick a random number < tot.
			auto x = rand_.int32() % tot;

			// Re-iterate through species and choose one.
			uint count = 0, id = 0;
			for(auto& s : species)
			{
				count += particlecomp_[s];
				if(count > x)
				{
					id = s;
					break;
				}
			}

			return DrawRandomParticleBySpecies(id);
		}

		// Draws a random particle by species from the world. 
		// Will return nullptr if the species doesn't exist.
		NewParticle* DrawRandomParticleBySpecies(uint species)
		{
			if((int)particlecomp_.size() - 1 < (int)species || particlecomp_[species] < 1)
				return nullptr;

			// Select random number between [0, count - 1].
			auto i = rand_.int32() % particlecomp_[species];
			return SelectParticleBySpecies(species, i);
		}

		// Selects the "ith" particle of species "species".
		NewParticle* SelectParticleBySpecies(uint species, int i)
		{
			auto it = std::find_if(particles_.begin(), particles_.end(), 
				[=](const NewParticle& p){
					return p.GetSpecies() == species;
				}
			);

			while(--i >= 0 && it != particles_.end())
			{
				++it; 
				it = std::find_if(it, particles_.end(), [=](const NewParticle& p){
					return p.GetSpecies() == species;

				});
			}

			return &(*it);
		}

		// Select particle by location.
		NewParticle* SelectParticle(uint i)
		{
			return &particles_[i];
		}

		// Add particle. 
		void AddParticle(const NewParticle&  p)
		{
			particles_.emplace_back(NewParticle(p, sites_));
			AddParticleComposition(particles_.back());
			particles_.back().SetIndex(particles_.size() - 1);
		}

		// Remove particle. 
		void RemoveParticle(const NewParticle& p)
		{
			// First remove compositions. 
			RemoveParticleComposition(p);

			// Now erase sites. 
			auto& ids = p.GetIndices();
			for(auto& i : ids)
				sites_.erase(sites_.begin() + i);

			// Finally delete particle.
			particles_.erase(particles_.begin() + p.GetIndex());
		}

		// Packs a world with the given particle blueprints and 
		// compositions to with "count" total particles and a 
		// specified density.
		void PackWorld(const std::vector<NewParticle*>& particles,
					   const std::vector<double>& fractions, 
					   int count, double density);

		// Get world volume.
		double GetVolume() const
		{
			return H_.determinant();
		}

		// Get world temperature.
		double GetTemperature() const { return temperature_; }

		// Sets world temperature.
		void SetTemperature(double temperature) { temperature_ = temperature; }

		// Get particle count.
		uint GetParticleCount() const
		{
			return particles_.size();
		}

		// Get site count.
		uint GetSiteCount() const
		{
			return sites_.size();
		}

		// Get neighbor cutoff radius.
		double GetNeighborRadius() const
		{
			return ncut_;
		}

		// Set neighbor cutoff radius.
		void SetNeighborRadius(double ncut)
		{
			ncut_ = ncut;
			ncutsq_ = ncut*ncut;
		}

		// Get particle compositions.
		const CompositionList& GetParticleCompositions() const
		{
			return particlecomp_;
		}

		// Get site compositions.
		const CompositionList& GetSiteCompositions() const
		{
			return sitecomp_;
		}

		// Gets/sets the periodicity of the x-coordinate.
		bool GetPeriodicX() const { return periodx_; }
		void SetPeriodicX(bool periodic) { periodx_ = periodic; }

		// Gets/sets the periodicity of the y-coordinate.
		bool GetPeriodicY() const { return periody_; }
		void SetPeriodicY(bool periodic) { periody_ = periodic; }

		// Gets/sets the periodicity of the z-coordinate.
		bool GetPeriodicZ() const { return periodz_; }
		void SetPeriodicZ(bool periodic) { periodz_ = periodic; }

		// Get cell ratio which is the fraction of neighbor cutoff 
		// that a cell is.
		double GetCellRatio() const
		{
			return cellratio_;
		}

		// Sets the size of a cell as a fraction of the neighbor 
		// cutoff radius.
		void SetCellRatio(double cratio)
		{
			cellratio_ = cratio;
		}

		// Generate cell list. Algorithm based on:
		// Heinz, T. N., Hunenberger, P. H. (2004). 
		// J. Comp. Chem., 25(12), 1474–1486. 
		// http://doi.org/10.1002/jcc.20071
		// Nomenclature generally follows paper.
		void GenerateCellList();

		// Get cell list pointer mask.
		const std::vector<uint>& GetMaskPtr() const
		{
			return pmask_;
		}

		// Get cell list vector.
		const std::vector<uint>& GetCellVector() const
		{
			return cell_;
		}

		// Get cell pointer vector.
		const std::vector<uint>& GetCellPointer() const
		{
			return cellptr_;
		}
	};
}