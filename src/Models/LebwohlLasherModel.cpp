#include "LebwohlLasherModel.h"

namespace Models
{
	// Configure a Lebwohl-Lasher mixture with a certian number of species and
	// targtet mole fractions. The species will be randomly distributed throughout
	// the lattice and the specified mole fractions will only be met approximately
	// since it may not be possible to get a perfect match.
	void LebwohlLasherModel::ConfigureMixture(int speciesCount,
	                                          std::vector<double> moleFractions)
	{
		if(moleFractions.size() != (unsigned int) speciesCount)
		{
			std::cerr <<
			"The number of specified mole fractions must equal the number of species."
			          <<
			std::endl;

			return;
		}

		// Normalize
		double norm = 0;
		for(int i = 0; i < speciesCount; i++)
			norm += moleFractions[i];

		for(int i = 0; i < speciesCount; i++)
			moleFractions[i] /= norm;

		// Initialize counts and specify species 1 as all.
		std::vector<double> counts(speciesCount, 0.0);
		int n = this->GetSiteCount();
		for(int i = 0; i < n; i++)
		{
			auto* site = this->SelectSite(i);
			counts[site->GetSpecies()-1]++;
		}

		// Loop through a few times to make sure we sample all points.
		for(int k = 0; k < 3; k++)
			for(int i = 0; i < n; i++)
			{
				auto* site = this->SelectRandomSite();
				int curr = site->GetSpecies();

				if(counts[curr-1]/n > moleFractions[curr-1])
					for(int j = 1; j <= speciesCount; j++)
						if(j != curr && counts[j-1]/n <
						   moleFractions[j-1])
						{
							counts[curr-1]--;
							site->SetSpecies(j);
							counts[j-1]++;
							break;
						}
			}
	}

	// Evaluate the LL Hamiltonian H = -Ʃ(γij + εij*P2(cosθij)).
	double LebwohlLasherModel::EvaluateHamiltonian(Site& site)
	{
		double h = 0;

		auto& si = site.GetUnitVectors();
		int alpha = site.GetSpecies();
		for(int &nindex : site.GetNeighbors())
		{
			auto& sj = Sites[nindex].GetUnitVectors();
			int beta = Sites[nindex].GetSpecies();

			// Dot product
			double dot = 0;
			for(size_t i = 0; i < sj.size(); i++)
				dot += si[i]*sj[i];

			// P2 Legendre polynomial
			h += this->GetIsotropicParameter(alpha, beta)
			     + this->GetInteractionParameter(alpha,beta)
			     *0.5*(3.0*dot*dot - 1.0);
		}

		return -1 * h;
	}

	// Gets the isotropic interaction parameter, γij, between two species.
	double LebwohlLasherModel::GetIsotropicParameter(int i, int j)
	{
		int n = _isotropicJ.size();

		// Sort max,min.
		int ni = (i > j) ? i : j;
		int nj = (i < j) ? i : j;

		return this->_isotropicJ[ni + ((2*n - nj)*(nj-1)/2) -1];
	}

	// Sets the isotropic interaction parameter, γij, between two species.
	double LebwohlLasherModel::SetIsotropicParameter(double e, int i, int j)
	{
		int n = _isotropicJ.size();

		// Sort max,min.
		int ni = (i > j) ? i : j;
		int nj = (i < j) ? i : j;

		// Calculate position.
		int p = ni + ((2*n - nj)*(nj-1)/2) -1;

		// Insert new location
		if(ni > n || nj > n)
			_isotropicJ.insert(
			        _isotropicJ.begin() + p, e);

		return this->_isotropicJ[p] = e;
	}
}
