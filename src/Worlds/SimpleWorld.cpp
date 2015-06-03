#include "SimpleWorld.h"

namespace SAPHRON
{
	// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
	// initialized with the appropriate composition.
	void SimpleWorld::ConfigureParticles(const std::vector<Particle*> particles,
	                        const std::vector<double> fractions)
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
					_particles.emplace_back(particles[j]->Clone());
					_particles[i]->SetPosition({x,y,z});
					counts[j]--;
					break;
				}

			// Increment counters.
			x += floor((y + z) / (_zlength + _ylength));
			y += floor(z / _zlength);
			z += 1.0;
		}
	}

	void SimpleWorld::UpdateNeighborList()
	{
		int n = this->GetParticleCount();

		// Clear neighbor list before repopulating.
		for(int i = 0; i < n; ++i)
			_particles[i]->ClearNeighborList();

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