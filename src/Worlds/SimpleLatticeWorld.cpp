#include "SimpleLatticeWorld.h"

namespace SAPHRON
{
	// Configure Particles in the lattice. For n particles and n fractions, the lattice will be
	// initialized with the appropriate composition.
	void SimpleLatticeWorld::ConfigureParticles(const std::vector<Particle*> particles,
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

	void SimpleLatticeWorld::ConfigureNeighborList()
	{
		if((int)_particles.size() != GetLatticeSize())
		{
			std::cerr << "ERROR: Please configure particles first." << std::endl;
			exit(-1);
		}

		for(int i = 0; i < GetLatticeSize()- 1; i++)
		{
			auto ni = _particles[i];
			auto ci = ni->GetPosition();

			for(int j = i + 1; j < GetLatticeSize(); j++)
			{
				auto nj = _particles[j];
				auto cj = nj->GetPosition();

				Position rij;
				rij.x = std::abs(ci.x - cj.x);
				rij.y = std::abs(ci.y - cj.y);
				rij.z = std::abs(ci.z - cj.z);

				if(rij.x == _xlength-1)
					rij.x = 1;
				if(rij.y == _ylength-1)
					rij.y = 1;
				if(rij.z == _xlength-1)
					rij.z = 1;

				if((rij.x + rij.y +rij.z) == 1)
				{
					ni->AddNeighbor(Neighbor(*nj));
					nj->AddNeighbor(Neighbor(*ni));
				}
			}
		}
	}

}