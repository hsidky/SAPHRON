#include "NewWorld.h"

namespace SAPHRON
{
	int NewWorld::nextID_ = 0;

	// Round to the nearest whole number. 
	// Adapeted from OpenMD 2.4.
	int anint(double x)
	{
		return (x >= 0.) ? static_cast<int>((x) + 0.5) : static_cast<int>((x) - 0.5);
	}

	// Quick and dirty square function
	template<typename T>
	T sq(T x)
	{
		return x*x;
	}

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

	// Generate cell list. Algorithm based on:
	// Heinz, T. N., Hunenberger, P. H. (2004). 
	// J. Comp. Chem., 25(12), 1474–1486. 
	// http://doi.org/10.1002/jcc.20071
	// Nomenclature generally follows paper.
	void NewWorld::GenerateCellList()
	{
		using std::abs;
		using std::min;
		using std::max;

		pmask_.clear();

		// Define number of cells in each direction. 
		auto Nx = static_cast<int>(H_(0,0)/(cellratio_*ncut_));
		auto Ny = static_cast<int>(H_(1,1)/(cellratio_*ncut_));
		auto Nz = static_cast<int>(H_(2,2)/(cellratio_*ncut_));
		auto M = Nx*Ny*Nz;

		// Compute grid cell edges (A.1).
		auto lx = H_(0,0)/Nx;
		auto ly = H_(1,1)/Ny;
		auto lz = H_(2,2)/Nz;

		// Loop through cell differences.
		int cdm = 1; // Last value of dm that was within neighbor radius.
		for(int dm = 1; dm < M; ++dm)
		{
			// Compute cell difference components (A.7-9).
			auto dmz = static_cast<int>(dm / (Nx * Ny));
			auto dmy = static_cast<int>((dm % (Nx * Ny)) / Nx);
			auto dmx = dm % Nx;

			// Compute minimum image cell distances (A.14-16).
			int dnx = abs(dmx);
			if(periodx_)
				dnx = abs(dmx - Nx*anint(static_cast<double>(dmx)/static_cast<double>(Nx)));
			
			int dny = abs(dmy);
			if(periody_)
			{
				int dny1 = abs(dmy - Ny*anint(static_cast<double>(dmy)/static_cast<double>(Ny)));
				int dny2 = abs((dmy + 1) - Ny*anint(static_cast<double>(dmy + 1)/static_cast<double>(Ny)));
				dny = min(dny1, dny2);
				if((dmx == 0) || ((dmy == Ny - 1) && (dmz == Nz - 1)))
					dny = dny1;
			}

			int dnz = abs(dmz);
			if(periodz_)
			{
				int dnz1 = abs(dmz - Nz*anint(static_cast<double>(dmz)/static_cast<double>(Nz)));
				int dnz2 = abs((dmz + 1) - Nz*anint(static_cast<double>(dmz + 1)/static_cast<double>(Nz)));
				dnz = min(dnz1, dnz2);
				if((dmz == Nz - 1) || (dmx == 0 && dmy == 0))
					dnz = dnz1;
			}

			// Compute minimum distance Rsq (A.5).
			auto Rsq = sq(max(dnx, 1) - 1)*lx*lx + sq(max(dny, 1) - 1)*ly*ly + sq(max(dnz, 1) - 1)*lz*lz;
			// Add to mask pointer vector.
			if(Rsq < ncutsq_)
			{
				if(cdm != dm - 1)
				{
					pmask_.push_back(dm);
					pmask_.push_back(dm);
				}
				else
					pmask_.back() = dm;
				cdm = dm;
			}
		}

		// Atoms per cell, heuristic from paper.
		int N = sites_.size();
		auto apc = max(1, static_cast<int>(1.5 * N / M));

		// Generate cell and cell pointer vectors. 
		cellptr_.resize(M + 1);
		cell_.resize(apc*M);
		for(int i = 0; i < M + 1; ++i)
			cellptr_[i] = i*apc;
		
		cellptr_[M] = N;

		for(int i = 0; i < N; ++i)
		{
			auto& p = sites_[i].position;
			
			// Get cell index of site (Eq. 5). Here we don't add 
			// 1 because indexing starts from 0 in array.
			auto m = 
			Nx*Ny*static_cast<int>(Nz*p[2]*Hinv_(2,2)) + 
			Nx*static_cast<int>(Ny*p[1]*Hinv_(1,1)) + 
			static_cast<int>(Nx*p[0]*Hinv_(0,0));

			cell_[cellptr_[m]] = i;
			assert(cellptr_[m] < m*apc + 1);
			++cellptr_[m];
		}

		// Compact cell vector and update cell pointer accordingly. 
		int j = cellptr_[0]; // Contains final filled entry in a cell.
		cellptr_[0] = 0; // This will inherently start at 0.
		for(int m = 1; m < M; ++m)
		{
			int k = cellptr_[m];
			cellptr_[m] = j; // Update cellptr with compact index.
			
			// Compact (move) entries from cell m up against
			// empty space.
			for(int i = apc * m; i < k; ++i)
			{
				cell_[j] = cell_[i];
				++j;
			}
		}
		cell_.resize(N);
	}
}