#pragma once

#include "Property.h"
#include <algorithm>
#include <armadillo>

namespace SAPHRON
{
	// Calculates the global nematic director as the highest
	// eigenvalue of the 2nd rank Q-tensor matrix.
	class GlobalDirector : public Property
	{
		private:
			arma::mat _u;
			std::vector<int> _idmap;

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
			GlobalDirector(const World& world) :
				Property(world),
				_u(world.GetParticleCount(), 3, arma::fill::zeros),
				_idmap(world.GetParticleCount(),0)
			{
				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto particle = world.SelectParticle(i);

					// Update ID map.
					_idmap[i] = particle->GetGlobalIdentifier();

					// Store director.
					auto vec = particle->GetDirector();
					for (int j = 0; j < 3; j++)
						_u(i, j) = vec[j];
				}
			}

			virtual void UpdateProperty(const ParticleList& particles) override
			{
				for(auto& particle : particles)
				{
					auto id = std::find(_idmap.begin(), _idmap.end(), particle->GetGlobalIdentifier());
					if(id != _idmap.end())
					{
						int index = id-_idmap.begin();
						auto& director = particle->GetDirectorRef();
						for(int j = 0; j < 3; ++j)
							_u(index, j) = director[j];
					}
				}
			}
	};
}
