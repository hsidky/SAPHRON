#include "SimpleWorld.h"

namespace SAPHRON
{
	void SimpleWorld::AddParticleComposition(Particle* particle)
	{
		int id = particle->GetSpeciesID();
		if(_composition.find(id) == _composition.end())
			_composition[id] = 1;
		else
			++_composition[id];

		for(auto& child : particle->GetChildren())
			AddParticleComposition(child);
	}

	void SimpleWorld::RemoveParticleComposition(Particle* particle)
	{
		int id = particle->GetSpeciesID();
		assert(_composition.find(id) != _composition.end());
		--_composition[id];

		for(auto& child : particle->GetChildren())
			RemoveParticleComposition(child);
	}

	void SimpleWorld::ModifyParticleComposition(const ParticleEvent& pEvent)
	{
		int oldID = pEvent.GetOldSpecies();
		int id = pEvent.GetParticle()->GetSpeciesID();
		assert(_composition.find(oldID) != _composition.end());
		--_composition[oldID];

		if(_composition.find(id) == _composition.end())
			_composition[id] = 1;
		else
			++_composition[id];
	}

	void SimpleWorld::BuildCompositionList()
	{
		for(auto& particle: _particles)
			AddParticleComposition(particle);
	}

	// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
	// initialized with the appropriate composition.
	void SimpleWorld::ConfigureParticles(const std::vector<Particle*> particles,
	                        const std::vector<double> fractions, int max)
	{
		if(particles.size() != fractions.size())
		{
			std::cerr << "ERROR: Particle and fraction count mismatch." << std::endl;
			exit(-1);
		}

		auto fracs = fractions;
		int cnt = (int)particles.size();

		// Normalize
		double norm = 0;
		for(int i = 0; i < cnt; i++)
			norm += fracs[i];

		for(int i = 0; i < cnt; i++)
			fracs[i] /= norm;

		// Initialize counts.
		std::vector<int> counts(cnt);
		for(int i = 0; i < cnt-1; i++)
			counts[i] = (int)std::round(fracs[i]*GetLatticeSize());

		counts[cnt-1] = GetLatticeSize();
		for(int i = 0; i < cnt-1; i++)
			counts[cnt-1] -= counts[i];

		// Loop though and initialize
		double x = 1;
		double y = 1;
		double z = 1;
		for(int i = 0; i < GetLatticeSize(); i++)
		{
			if(z > _zlength)
				z = 1.0;
			if(y > _ylength)
				y = 1.0;
			if(x > _xlength)
				x = 1.0;

			for(int j = 0; j < cnt; j++)
				if(counts[j] > 0 )
				{
					Particle* pnew = particles[j]->Clone();
					pnew->SetPosition({x,y,z});
					AddParticle(pnew);
					--counts[j];
					break;
				}

			// Increment counters.
			x += floor((y + z) / (round(_zlength) + round(_ylength)));
			y += floor(z / round(_zlength));
			z += 1.0;

			if(max != 0 && i >= max - 1)
				return;
		}
	}

	void SimpleWorld::UpdateNeighborList()
	{
		int n = this->GetParticleCount();

		// Clear neighbor list before repopulating.
		for(int i = 0; i < n; ++i)
		{
			_particles[i]->ClearNeighborList();
			_particles[i]->SetCheckpoint();
		}

		for(int i = 0; i < n - 1; ++i)
		{
			auto* pi = _particles[i];
			auto posi = pi->GetPosition();

			for(int j = i + 1; j < n; ++j)
			{
				auto pj = _particles[j];
				auto posj = pj->GetPositionRef();

				Position dist = posi - posj;

				dist.x -= _xlength*anint(dist.x/_xlength);
				dist.y -= _ylength*anint(dist.y/_ylength);
				dist.z -= _zlength*anint(dist.z/_zlength);

				if(dist.norm() <= _ncut)
				{
					pi->AddNeighbor(pj);
					pj->AddNeighbor(pi);
				}
			}
		}
	}

}