#include "ForceFieldManager.h"
#include "../Particles/NewParticle.h"
#include "../Worlds/NewWorld.h"

namespace SAPHRON
{
	void ForceFieldManager::AddNonBondedForceField(uint s1, uint s2, const NonBondedFF& ff)
	{
		// Resize and update vector / count if necessary.
		auto smax = std::max(s1, s2) + 1;
		if(smax > nbcount_)
		{
			nbcount_ = smax;
			nonbondedffs_.resize(smax*smax, nullptr);
		}

		// Get mapped index with symmetry.
		auto idx1 = GetIndex(s1, s2);
		auto idx2 = GetIndex(s2, s1);

		nonbondedffs_[idx1] = &ff;
		nonbondedffs_[idx2] = &ff;
	}

	void ForceFieldManager::RemoveNonBondedForceField(uint s1, uint s2)
	{
		auto smax = std::max(s1, s2) + 1;

		if(smax <= nbcount_)
		{
			auto idx1 = GetIndex(s1, s2);
			auto idx2 = GetIndex(s2, s1);
			nonbondedffs_[idx1] = nullptr;
			nonbondedffs_[idx2] = nullptr;
		}
	}

	double ForceFieldManager::EvaluateInterEnergy(const NewParticle& p, const NewWorld& w) const
	{
		double u = 0.;
		for(size_t i = 0; i < p.SiteCount(); ++i)
			u += EvaluateInterEnergy(p.GetSite(i), w);
		return u;
	}

	double ForceFieldManager::EvaluateInterEnergy(const Site& s, const NewWorld& w) const
	{
		double u = 0.;

		// Get appropriate references.
		auto& sites = w.GetSites();
		auto& C = w.GetCellVector();
		auto& Pc = w.GetCellPointer();
		auto& Pm = w.GetMaskPointer();
		
		auto wid = w.GetID();
		// Get current cell and loop through all interacting stripes.
		auto S = w.GetStripeCount();
		int mi = w.GetCellIndex(s.position);

		// Evaluate current cell. 
		for(auto i = Pc[mi]; i < Pc[mi + 1]; ++i)
		{
			auto& sj = sites[C[i]];
			if(s.pid == sj.pid)
				continue;

			// Get site-site distance.
			Vector3 rij = s.position - sj.position;
			w.ApplyMinimumImage(rij);
			auto rsq = rij.squaredNorm();

			// Get forcefield index and evaluate nonbonded. 
			auto idx = GetIndex(s.species, sj.species);
			auto& ff = nonbondedffs_[idx];
			if(ff != nullptr)
				u += ff->EvaluateEnergy(s, sj, rij, rsq, wid);
		}

		#pragma omp parallel for reduction(+:u)
		for(int i = 0; i < S; ++i)
		{
			// First and last cells of stripe. 
			auto m1 = mi + Pm[2*i];
			auto m2 = mi + Pm[2*i+1];
			for(auto l = Pc[m1]; l < Pc[m2 + 1]; ++l)
			{
				auto& sj = sites[C[l]];

				// Skip species with same parent (intramolecular).
				if(s.pid == sj.pid)
					continue;

				// Get site-site distance.
				Vector3 rij = s.position - sj.position;
				w.ApplyMinimumImage(rij);
				auto rsq = rij.squaredNorm();

				// Get forcefield index and evaluate nonbonded. 
				auto idx = GetIndex(s.species, sj.species);
				auto& ff = nonbondedffs_[idx];
				if(ff != nullptr)
					u += ff->EvaluateEnergy(s, sj, rij, rsq, wid);					
			}
		}

		return u;
	}

	double ForceFieldManager::EvaluateInterEnergy(const NewWorld& w) const
	{
		double u = 0.;
		for(auto& s : w.GetSites())
			u += EvaluateInterEnergy(s, w);

		return u/2.;
	}

	double ForceFieldManager::EvaluateTailEnergy(const NewWorld& w) const
	{
		auto& comp = w.GetSiteCompositions();
		auto wid = w.GetID();
		auto v = w.GetVolume();

		double u = 0.;
		// Go through unique pairs of species.
		for(uint i = 0; i < nbcount_; ++i)
			for(uint j = i; j < nbcount_; ++j)
			{
				auto na = comp[i];
				auto nb = comp[j];
				auto idx = GetIndex(i, j);

				u += na*nb*nonbondedffs_[idx]->EnergyTailCorrection(wid);
			}

		//u.pressure *= 2.*M_PI/(3.*v*v);

		return u*2.*M_PI/v;
	}

	Matrix3 ForceFieldManager::EvaluateVirial(const NewParticle& p, const NewWorld& w) const
	{
		Matrix3 v = Matrix3::Zero();
		for(size_t i = 0; i < p.SiteCount(); ++i)
			v += EvaluateVirial(p.GetSite(i), w);
		return v;
	}

	Matrix3 ForceFieldManager::EvaluateVirial(const Site& s, const NewWorld& w) const
	{
		Matrix3 v = Matrix3::Zero();

		// Get appropriate references.
		auto& particles = w.GetParticles();
		auto& sites = w.GetSites();
		auto& C = w.GetCellVector();
		auto& Pc = w.GetCellPointer();
		auto& Pm = w.GetMaskPointer();
		
		auto wid = w.GetID();
		// Get current cell and loop through all interacting stripes.
		auto S = w.GetStripeCount();
		int mi = w.GetCellIndex(s.position);

		// Evaluate current cell. 
		for(auto i = Pc[mi]; i < Pc[mi + 1]; ++i)
		{
			auto& sj = sites[C[i]];
			if(s.pid == sj.pid)
				continue;

			// Get site-site distance.
			Vector3 rij = s.position - sj.position;
			w.ApplyMinimumImage(rij);
			auto rsq = rij.squaredNorm();

			// Get molecule-molecule distance.
			Vector3 rab = particles[s.pid].GetPosition() - particles[sj.pid].GetPosition();
			w.ApplyMinimumImage(rab);

			// Get forcefield index and evaluate nonbonded. 
			auto idx = GetIndex(s.species, sj.species);
			auto& ff = nonbondedffs_[idx];
			if(ff != nullptr)
				v.noalias() += ff->EvaluateForce(s, sj, rij, rsq, wid)*rab.transpose();
		}

		//#pragma omp declare reduction (+ : Matrix3 : omp_out += omp_in ) initializer (omp_priv=Matrix3::Zero())
		//#pragma omp parallel for reduction(+:v) schedule(static)
		for(int i = 0; i < S; ++i)
		{
			// First and last cells of stripe. 
			auto m1 = mi + Pm[2*i];
			auto m2 = mi + Pm[2*i+1];
			for(auto l = Pc[m1]; l < Pc[m2 + 1]; ++l)
			{
				auto& sj = sites[C[l]];

				// Skip species with same parent (intramolecular).
				if(s.pid == sj.pid)
					continue;

				// Get site-site distance.
				Vector3 rij = s.position - sj.position;
				w.ApplyMinimumImage(rij);
				auto rsq = rij.squaredNorm();

				// Get molecule-molecule distance.
				Vector3 rab = particles[s.pid].GetPosition() - particles[sj.pid].GetPosition();
				w.ApplyMinimumImage(rab);

				// Get forcefield index and evaluate nonbonded. 
				auto idx = GetIndex(s.species, sj.species);
				auto& ff = nonbondedffs_[idx];
				if(ff != nullptr)
					v.noalias() += ff->EvaluateForce(s, sj, rij, rsq, wid)*rab.transpose();					
			}
		}

		return -1.*v;
	}

	Matrix3 ForceFieldManager::EvaluateVirial(const NewWorld& w) const
	{
		Matrix3 u = Matrix3::Zero();
		for(auto& s : w.GetSites())
			u += EvaluateVirial(s, w);

		return u/2.;
	}

	double ForceFieldManager::EvaluateTailPressure(const NewWorld& w) const
	{
		auto& comp = w.GetSiteCompositions();
		auto wid = w.GetID();
		auto v = w.GetVolume();

		double p = 0.;
		// Go through unique pairs of species.
		for(uint i = 0; i < nbcount_; ++i)
			for(uint j = i; j < nbcount_; ++j)
			{
				auto na = comp[i];
				auto nb = comp[j];
				auto idx = GetIndex(i, j);

				p += na*nb*nonbondedffs_[idx]->PressureTailCorrection(wid);
			}

		return p*2.*M_PI/(3.*v*v);
	}

}