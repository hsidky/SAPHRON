#pragma once 
#include <Eigen/Dense>
#include <iostream>

namespace SAPHRON
{
	using Vector3 = Eigen::Vector3d; 	
	using BondMatrix = Eigen::MatrixXi;

	// Structure for interaction site.
	struct Site
	{
		Vector3 position;
		Vector3 director = {0, 0, 1};
		uint species;
		double charge = 0.0;
		double mass = 1.0;
	};

	using SiteList = std::vector<Site*>;

	class NewParticle
	{
	private:
		SiteList sites_;
		Vector3 position_;
		double mass_;
		double charge_;
		uint species_;

		// Adjacency matrix for bonds.
		BondMatrix bonds_;

	public:
		NewParticle(uint species, const SiteList& plist) : 
		sites_(plist), position_{0, 0, 0}, mass_(0), charge_(0),
		species_(species), bonds_(plist.size(), plist.size())
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
			for(auto& p : sites_)
				p->position += pos;

			position_ = position;
		}

		// Get position of individual site.
		const Vector3& GetPosition(uint i) const
		{
			return sites_[i]->position;
		}

		// Set position of individual site.
		void SetPosition(uint i, const Vector3& pos)
		{
			auto& data = sites_[i];
			auto dp = pos - data->position;
			position_ += data->mass/mass_*dp;
			data->position = pos;
		}

		// Get overall particle charge.
		double GetCharge() const { return charge_; }

		// Return charge of individual site.
		double GetCharge(uint i) const { return sites_[i]->charge; }

		// Set charge of individual site.
		void SetCharge(uint i, double q) 
		{
			auto& data = sites_[i];
			auto dc = q - data->charge;
			charge_ += dc;
			data->charge = q;
		}

		// Get director of individual site.
		const Vector3& GetDirector(uint i) const { return sites_[i]->director; }

		// Set director of individual site.
		void SetDirector(uint i, const Vector3& dir)
		{
			sites_[i]->director = dir;
		}

		// Get particle mass.
		double GetMass() const { return mass_; }

		// Get mass of individual site.
		double GetMass(uint i) const { return sites_[i]->mass; }

		// Set mass of individual site.
		void SetMass(uint i, double mass)
		{
			auto& data = sites_[i];
			auto dm = mass - data->mass;
			mass_ += dm;
			data->mass = mass;
		}

		// Get speices ID.
		uint GetSpecies() { return species_; }

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
		uint SiteCount() const { return sites_.size(); }

		// Update particle COM, charge and mass.
		void UpdateConfiguration()
		{
			mass_ = 0;
			charge_ = 0;
			position_ = {0, 0, 0};
			for(auto& p : sites_)
			{
				position_ += p->position;
				mass_ += p->mass;
				charge_ += p->charge;
			}

			position_ /= mass_;
		}

		// Iterators.
		using const_iterator = SiteList::const_iterator;
		const_iterator begin() const { return sites_.begin(); }
		const_iterator end() const { return sites_.end(); }
	};
}