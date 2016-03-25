#include "NewWorld.h"

namespace SAPHRON
{
	int NewWorld::nextID_ = 0;

	void NewWorld::PackWorld(const std::vector<NewParticle*>& particles,
							 const std::vector<double>& fractions, 
							 int n, double density)
	{

		if(particles.size() != fractions.size())
		{
			std::cerr << "ERROR: Particle and fraction count mismatch." << std::endl;
			exit(-1);
		}

		auto fracs = fractions;
		int nspecies = (int)particles.size();

		// Normalize
		double norm = 0;
		for(int i = 0; i < nspecies; i++)
			norm += fracs[i];

		for(int i = 0; i < nspecies; i++)
			fracs[i] /= norm;

		// Initialize counts.
		std::vector<int> counts(nspecies);
		for(int i = 0; i < nspecies-1; i++)
			counts[i] = (int)std::round(fracs[i]*n);

		// Make sure rounding above works out to perfect numbers.
		counts[nspecies-1] = n;
		for(int i = 0; i < nspecies-1; i++)
			counts[nspecies-1] -= counts[i];

		// New volume.
		double vn = (double)n/density;
		H_ *= std::cbrt(vn/GetVolume());

		// Get corresponding box size.
		double L = std::cbrt(vn);

		// Find the lowest perfect cube greater than or equal to the number of particles.
		int nCube = 2;
		while(pow(nCube, 3) < n)
			++nCube;

		// Coordinates.
		int x = 0;
		int y = 0;
		int z = 0;

		// Allocate memory. 
		int ns = 0;
		for(int i = 0; i < nspecies; ++i)
			ns += particles[i]->SiteCount()*counts[i];

		sites_.reserve(sites_.size() + ns);
		particles_.reserve(particles_.size() + n);

		// Assign particle positions.
		for (int i = 0; i < n; ++i)
		{
			for(int j = 0; j < nspecies; j++)
				if(counts[j] > 0 )
				{
					AddParticle(*particles[j]);
					auto& pnew = particles_.back();
					double mult = (L/nCube);
					pnew.SetPosition({mult*(x+0.5),mult*(y+0.5),mult*(z+0.5)});
					--counts[j];
					break;
				}

			if(++x == nCube)
			{	
				x = 0;
				if(++y == nCube)
				{
					y = 0;
					++z;
				}
			}
		}
	}
}