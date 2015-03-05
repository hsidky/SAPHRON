#include "LebwohlLasherModel.h"
#include <iomanip>

namespace Models
{
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
			for(size_t i = 0; i < 3; i++)
				dot += si[i]*sj[i];

			// P2 Legendre polynomial
			h += this->GetIsotropicParameter(alpha, beta)
			     + this->GetInteractionParameter(alpha,beta)
			     *(1.5*dot*dot - 0.5);
		}

		return -1 * h;
	}

	// Gets the isotropic interaction parameter, γij, between two species.
	double LebwohlLasherModel::GetIsotropicParameter(int i, int j)
	{
		// Sort max,min.
		if(i < j)
			std::swap(i, j);

		return this->_isotropicJ[i + ((2*_isoN - j)*(j-1)/2) -1];
	}

	// Sets the isotropic interaction parameter, γij, between two species.
	// If a value of of i or j is greater than the existing largest interaction
	// parameter "N", then memory is allocated for all values in between "N" and
	// max(i,j). So for example if N = 2 (a11 a12 a22) and i = 3, then the new
	// set of interaction parameters are (a11 a12 a13 a22 a23 a33).
	double LebwohlLasherModel::SetIsotropicParameter(double e, int i, int j)
	{
		int n = (int)_isotropicJ.size();

		// Sort max,min.
		int ni = (i > j) ? i : j;
		int nj = (i < j) ? i : j;

		// Resize vector if needed.
		int nnew = ni*(ni+1)/2;
		if(nnew > n)
		{
			// Store old vars.
			auto oldV = _isotropicJ;
			int oldN = _isoN;

			// Resize and update column length.
			_isotropicJ.resize(nnew);
			_isoN = ni;

			// Update all of the previous entries
			for(int k = 1; k <= oldN; k++)
				for(int l = 1; l <= oldN; l++)
				{
					int nk = (k > l) ? k : l;
					int nl = (k < l) ? k : l;
					int p = nk + ((2*oldN - nl)*(nl-1)/2) -1;
					SetIsotropicParameter(oldV[p], nk, nl);
				}
		}

		// Calculate position.
		int p = ni + ((2*_isoN - nj)*(nj-1)/2) -1;

		std::cout << "Isotropic interaction parameter set\n"
		          << "Species: " << i << " & " << j << std::fixed << std::setprecision(5)
		          << "  Value: " << e << "\n" << std::endl;

		return this->_isotropicJ[p] = e;
	}
}
