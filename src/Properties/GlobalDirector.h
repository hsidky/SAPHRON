#pragma once

#include "Property.h"
#include <armadillo>

namespace Properties
{
	// Calculates the global nematic director as the highest
	// eigenvalue of the 2nd rank Q-tensor matrix.
	class GlobalDirector : public Property
	{
		private:
			arma::mat _u;

			std::vector<double> ComputeDirector()
			{
				auto Q = 3.0 / (2.0*_u.n_rows)*(_u.t()*_u - 1.0 / 3.0 * arma::eye(3, 3));
				arma::cx_vec eigval;
				arma::cx_mat eigvec;
				arma::eig_gen(eigval, eigvec, Q);

				arma::uword imax;
				eigval.max(imax);
				return arma::conv_to<std::vector<double> >::from(eigvec.col(imax));
			}

		public:
			GlobalDirector(const BaseModel& model) :
				Property(model), _u(model.GetSiteCount(), 3, arma::fill::zeros)
			{
				for (int i = 0; i < model.GetSiteCount(); i++)
				{
					auto site = model.SelectSite(i);
					auto vec = site->GetUnitVectors();
					for (int j = 0; j < 3; j++)
						_u(i, j) = vec[j];
				}
			}

			virtual void CalculateProperty(const BaseModel& model, int index)
			{
				for (int i = 0; i < model.GetSiteCount(); i++)
				{
					auto site = model.SelectSite(i);
					auto vec = site->GetUnitVectors();
					for (int j = 0; j < 3; j++)
						_u(i, j) = vec[j];
				}
			}
	};
}
