#pragma once 

#include <Eigen/Dense>
#include <iostream>
#include "../Utils/Types.h"

namespace SAPHRON
{
	// Structure for interaction site.
	struct Site
	{
		Vector3 position = {0, 0, 0};
		Vector3 director = {0, 0, 1};
		uint species = 0;
		uint pid = 0;
		// To be used by cell list manager, not forcefield manager.
		uint cellid = 0;
		double charge = 0.0;
		double mass = 1.0;
	};

	class NewParticle
	{
	private:
		std::vector<Site>* sites_;
		IndexList indices_;
		Vector3 position_;
		double mass_;
		double charge_;
		uint species_;

		// Optional particle index in container.
		uint idx_; 

		// Adjacency matrix for bonds.
		BondMatrix bonds_;

	public:
		// Copy particle and store sites in container reference provided.
		NewParticle(const NewParticle& p, std::vector<Site>& sites) : 
		sites_(&sites), indices_(0), position_(p.position_), mass_(p.mass_), 
		charge_(p.charge_), species_(p.species_), idx_(0), bonds_(p.bonds_) 
		{
			sites.reserve(sites.size() + p.SiteCount());
			// Copy sites.
			for(uint i = 0; i < p.indices_.size(); ++i)
			{
				sites.emplace_back((*p.sites_)[p.indices_[i]]);
				indices_.push_back(sites.size() - 1);
			}
			UpdateConfiguration();
		}

		// New particle of type species with sites contained in sites vector at locations
		// specified in indices.
		NewParticle(uint species, const IndexList& indices, std::vector<Site>* sites) : 
		sites_(sites), indices_(indices), position_{0, 0, 0}, mass_(0), charge_(0),
		species_(species), idx_(0), bonds_(sites->size(), sites->size())
		{
			bonds_.fill(0);
			UpdateConfiguration();
		}

		// Get particle position.
		Vector3 GetPosition() const { return position_; }

		// Set particle position.
		void SetPosition(const Vector3& position)
		{
			auto pos = position - position_;
			for(auto& i : indices_)
				(*sites_)[i].position += pos;

			position_ = position;
		}

		// Get position of individual site.
		const Vector3& GetPosition(uint i) const
		{
			return (*sites_)[indices_[i]].position;
		}

		// Set position of individual site.
		void SetPosition(uint i, const Vector3& pos)
		{
			auto& data = (*sites_)[indices_[i]];
			auto dp = pos - data.position;
			position_ += data.mass/mass_*dp;
			data.position = pos;
		}

		// Get overall particle charge.
		double GetCharge() const { return charge_; }

		// Return charge of individual site.
		double GetCharge(uint i) const 
		{ 
			return (*sites_)[indices_[i]].charge; 
		}

		// Set charge of individual site.
		void SetCharge(uint i, double q) 
		{
			auto& data = (*sites_)[indices_[i]];
			auto dc = q - data.charge;
			charge_ += dc;
			data.charge = q;
		}

		// Get director of individual site.
		const Vector3& GetDirector(uint i) const 
		{ 
			return (*sites_)[indices_[i]].director; 
		}

		// Set director of individual site.
		void SetDirector(uint i, const Vector3& dir)
		{
			(*sites_)[indices_[i]].director = dir;
		}

		// Get particle mass.
		double GetMass() const { return mass_; }

		// Get mass of individual site.
		double GetMass(uint i) const 
		{ 
			return (*sites_)[indices_[i]].mass; 
		}

		// Set mass of individual site.
		void SetMass(uint i, double mass)
		{
			auto& data = (*sites_)[indices_[i]];
			auto dm = mass - data.mass;
			mass_ += dm;
			data.mass = mass;
		}

		// Get speices ID.
		uint GetSpecies() const { return species_; }

		// Get site i species ID.
		uint GetSpecies(uint i) const
		{
			return (*sites_)[indices_[i]].species;
		}

		// Get const. reference to site.
		const Site& GetSite(uint i) const
		{
			return (*sites_)[indices_[i]];
		}

		// Get site indices.
		const IndexList& GetIndices() const
		{
			return indices_;
		}

		// Gets container index.
		uint GetIndex() const
		{
			return idx_;
		}

		// Sets container index and parent id in sites.
		void SetIndex(uint idx)
		{
			idx_ = idx;
			for(auto& i : indices_)
				(*sites_)[i].pid = idx;
		}

		// Adds a bond between sites i and j.
		void AddBond(uint i, uint j)
		{
			bonds_(i, j) = 1;
			bonds_(j, i) = 1;
		}

		// Removes a bond between sites i and j.
		void RemoveBond(uint i, uint j)
		{
			bonds_(i, j) = 0;
			bonds_(j, i) = 0;
		}

		// Gets the bond matrix.
		const BondMatrix& GetBonds() const
		{
			return bonds_;
		}

		// Determines if sites i and j are bonded.
		bool IsBonded(uint i, uint j) const
		{
			return bonds_(i, j) == 1;
		}

		// Get number of sites in particle.
		uint SiteCount() const { return indices_.size(); }

		// Update particle COM, charge and mass.
		void UpdateConfiguration()
		{
			mass_ = 0;
			charge_ = 0;
			position_ = {0, 0, 0};
			for(auto& i : indices_)
			{
				position_ += (*sites_)[i].position;
				mass_ += (*sites_)[i].mass;
				charge_ += (*sites_)[i].charge;
			}

			position_ /= mass_;
		}
	};
}