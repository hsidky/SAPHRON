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

	EP ForceFieldManager::EvaluateInterEnergy(const Site& s, const NewWorld& w) const
	{
		EP ep;

		// Get appropriate references.
		auto& particles = w.GetParticles();
		auto& sites = w.GetSites();
		auto& C = w.GetCellVector();
		auto& Pc = w.GetCellPointer();
		auto& Pm = w.GetMaskPointer();
		
		auto wid = w.GetID();
		// Get current cell and loop through all interacting stripes.
		auto S = w.GetStripeCount();
		auto M = w.GetCellCount();
		auto mi = w.GetCellIndex(s.position);

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
			{
				auto ef = ff->Evaluate(s, sj, rij, rsq, wid);
				ep.intervdw += ef.energy;
				ep.virial.noalias() -= ef.force*rab.transpose();
			}
		}

		for(int i = 0; i < S; ++i)
		{
			// First and last cells of stripe. 
			auto m1 = mi + Pm[2*i];
			auto m2 = mi + Pm[2*i+1];

			// Go through each stripe.
			for(auto j = m1; j < m2 + 1; ++j)
			{
				// Wrap stripe index around.
				auto k = j - (int)(j/M)*M;
				for(auto l = Pc[k]; l < Pc[k + 1]; ++l)
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
					{
						auto ef = ff->Evaluate(s, sj, rij, rsq, wid);
						ep.intervdw += ef.energy;
						ep.virial.noalias() -= ef.force*rab.transpose();
					}
				}
			}
		}

		return ep;
	}

	EP ForceFieldManager::EvaluateInterEnergy(const NewWorld& w) const
	{
		EP u;
		for(auto& s : w.GetSites())
			u += EvaluateInterEnergy(s, w);

		u.intervdw *= 0.5;
		u.virial *= 0.5;

		return u;
	}
}