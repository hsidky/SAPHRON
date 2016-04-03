#include "NewWorld.h"
#include "json/json.h"
#include "../Particles/BlueprintManager.h"
#include "../Simulation/SimException.h"
#include "../Validator/ObjectRequirement.h"
#include "schema.h"
#include <random>
#include <algorithm>

using namespace Json;

namespace SAPHRON
{
	uint NewWorld::nextID_ = 0;

	// Round to the nearest whole number. 
	// Adapeted from OpenMD 2.4.
	int nint(double x)
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
		H2_ = H_/2;
		Hinv_ = H_.inverse();

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
	void NewWorld::BuildCellList()
	{
		using std::abs;
		using std::min;
		using std::max;

		pmask_.clear();

		// Define number of cells in each direction. 
		Nx_ = static_cast<int>(H_(0,0)/(cellratio_*ncut_));
		Ny_ = static_cast<int>(H_(1,1)/(cellratio_*ncut_));
		Nz_ = static_cast<int>(H_(2,2)/(cellratio_*ncut_));
		auto M = Nx_*Ny_*Nz_;
		ccount_ = M;

		// Compute grid cell edges (A.1).
		auto lx = H_(0,0)/Nx_;
		auto ly = H_(1,1)/Ny_;
		auto lz = H_(2,2)/Nz_;

		// Loop through cell differences.
		int cdm = 1; // Last value of dm that was within neighbor radius.
		for(int dm = 1; dm < M; ++dm)
		{
			// Compute cell difference components (A.7-9).
			auto dmz = static_cast<int>(dm / (Nx_ * Ny_));
			auto dmy = static_cast<int>((dm % (Nx_ * Ny_)) / Nx_);
			auto dmx = dm % Nx_;

			// Compute minimum image cell distances (A.14-16).
			int dnx = abs(dmx);
			if(periodx_)
				dnx = abs(dmx - Nx_*nint(static_cast<double>(dmx)/static_cast<double>(Nx_)));
			
			int dny = abs(dmy);
			if(periody_)
			{
				int dny1 = abs(dmy - Ny_*nint(static_cast<double>(dmy)/static_cast<double>(Ny_)));
				int dny2 = abs((dmy + 1) - Ny_*nint(static_cast<double>(dmy + 1)/static_cast<double>(Ny_)));
				dny = min(dny1, dny2);
				if((dmx == 0) || ((dmy == Ny_ - 1) && (dmz == Nz_ - 1)))
					dny = dny1;
			}

			int dnz = abs(dmz);
			if(periodz_)
			{
				int dnz1 = abs(dmz - Nz_*nint(static_cast<double>(dmz)/static_cast<double>(Nz_)));
				int dnz2 = abs((dmz + 1) - Nz_*nint(static_cast<double>(dmz + 1)/static_cast<double>(Nz_)));
				dnz = min(dnz1, dnz2);
				if((dmz == Nz_ - 1) || (dmx == 0 && dmy == 0))
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

		scount_ = pmask_.size()/2;
		UpdateCellList();
		BuildNeighborList();
	}

	void NewWorld::UpdateCellList()
	{
		using std::max;

		// Atoms per cell, heuristic from paper.
		int N = sites_.size();
		//auto apc = max(2, static_cast<int>(2.0 * N / ccount_));
		auto apc = static_cast<int>(2.0 * N / ccount_) + 1;

		// Generate cell and cell pointer vectors. 
		cellptr_.resize(2*(ccount_ + 1));
		cell_.resize(apc*ccount_);

		for(uint i = 0; i < ccount_ + 1; ++i)
			cellptr_[i] = i*apc;
		
		cellptr_[ccount_] = N;

		for(int i = 0; i < N; ++i)
		{
			// Clear the neighbor list.
			sites_[i].neighbors.clear();

			// Get cell index of site.
			auto m = GetCellIndex(sites_[i].position);
			sites_[i].idx = i;
			cell_[cellptr_[m]] = i;
			assert(cellptr_[m] < m*apc + 1);
			++cellptr_[m];
		}

		// Compact cell vector and update cell pointer accordingly. 
		int j = cellptr_[0]; // Contains final filled entry in a cell.
		cellptr_[0] = 0; // This will inherently start at 0.
		for(uint m = 1; m < ccount_; ++m)
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

	void NewWorld::BuildNeighborList()
	{
		// Loop over all cells.
		for(uint m = 0; m < ccount_; ++m)
		{
			// Loop over all primary atoms in cell.
			for(auto n1 = cellptr_[m]; n1 < cellptr_[m + 1]; ++n1)
			{
				auto i = cell_[n1];
				auto& s1 = sites_[i];
				s1.checkpoint = s1.position;

				// Loop over secondary atoms in cell.
				for(auto n2 = n1 + 1; n2 < cellptr_[m + 1]; ++n2)
				{
					auto j = cell_[n2];
					auto& s2 = sites_[j];
					s2.checkpoint = s2.position;
					if(s1.pid != s2.pid)
					{
						s1.neighbors.push_back(j);
						s2.neighbors.push_back(i);
					}
				}
				
				// Loop over all stripes.
				for(uint s = 0; s < scount_; ++s)
				{
					auto m1 = m + pmask_[2*s];
					if(m1 < ccount_)
					{
						auto m2 = std::min(m + pmask_[2*s + 1], ccount_ - 1);
						for(auto n2 = cellptr_[m1]; n2 < cellptr_[m2 + 1]; ++n2)
						{
							auto j = cell_[n2];
							auto& s2 = sites_[j];
							Vector3 r12 = s1.position - s2.position;
							ApplyMinimumImage(r12);
							if(r12.squaredNorm() < ncutsq_)
							{
								if(s1.pid != s2.pid)
								{
									s1.neighbors.push_back(j);
									s2.neighbors.push_back(i);
								}
							}
						}
					}
				}
			}
		}
	}

	std::shared_ptr<NewWorld> NewWorld::Build(const Json::Value& json)
	{
		using std::to_string;

		ObjectRequirement validator;
		Value schema;
		Reader reader;

		// Parse schema.
		reader.parse(JsonSchema::SimpleWorld, schema);
		validator.Parse(schema, "#/worlds");

		// Validate input.
		validator.Validate(json, "#/worlds");

		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		Vector3 dim{
			json["dimensions"][0].asDouble(), 
			json["dimensions"][1].asDouble(),
			json["dimensions"][2].asDouble()
		};

		// Neighbor cutoff and cell ratio.
		auto ncut = json["ncut"].asDouble();
		auto cellratio = json.get("cell_ratio", 0.2).asDouble();
		auto skin = json["skin"].asDouble();

		// Get seed.
		std::random_device rd;
		auto maxi = std::numeric_limits<int>::max();
		auto seed = json.get("seed", rd() % maxi).asUInt();

		std::shared_ptr<NewWorld> world = std::make_shared<NewWorld>(dim[0], dim[1], dim[2], ncut, skin, seed);

		// Cell ratio
		world->SetCellRatio(cellratio);

		// Periodic. 
		bool periodx = true, periody = true, periodz = true;
		if(json.isMember("periodic"))
		{
			periodx = json["periodic"].get("x", true).asBool();
			periody = json["periodic"].get("y", true).asBool();
			periodz = json["periodic"].get("z", true).asBool();
		}
		world->SetPeriodicX(periodx);
		world->SetPeriodicY(periody);
		world->SetPeriodicZ(periodz);

		// Go through components and add particles to world. 
		int k = 0;
		std::vector<Site> sites;
		auto& bp = BlueprintManager::Instance();
		for(auto& c : json["components"])
		{
			auto species = c[0].asString();
			auto count = c[1].asInt();

			// Get protoype from blueprint.
			sites.clear();
			if(!bp.IsRegisteredBlueprint(species))
				throw BuildException({"Blueprint of species \"" + species + "\" does not exist."});

			// Make sure we have enough remaining.
			auto rm = (int)json["particles"].size() - k;
			if(rm < count)
				throw BuildException({"#/worlds/i/components/" + species + 
					": Expected " + to_string(count) + " but got " + to_string(rm) + "."});

			auto proto = bp.GetBlueprint(species, sites);
			for(int j = 0; j < count; ++j)
			{
				for(uint i = 0; i < proto.SiteCount(); ++i)
				{
					// Resolve expected species string.
					auto sstr = bp.GetSpeciesString(proto.GetSpecies(i));

					// Get json particle and compare.
					auto& p = json["particles"][k];
					auto ps = p[1].asString();
					if(ps != sstr)
						throw BuildException({"#/worlds/i/particles/" + to_string(k) + 
							": Expected \"" + sstr + "\" but got \"" + ps + "\"."});

					// Set site properties.
					proto.SetPosition(i, {p[2][0].asDouble(), p[2][1].asDouble(), p[2][2].asDouble()});

					if(p.size() > 3)
						proto.SetDirector(i, {p[3][0].asDouble(), p[3][1].asDouble(), p[3][2].asDouble()});

					++k;
				}
				world->AddParticle(proto);
			}
		}

		return world;
	}
}