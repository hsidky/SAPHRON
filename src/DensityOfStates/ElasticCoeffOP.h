#pragma once 

#include <armadillo>
#include <functional>
#include <map>

#include "DOSOrderParameter.h"
#include "../Particles/ParticleObserver.h"
#include "../Worlds/World.h"

namespace SAPHRON
{
	typedef std::function<bool(const Particle*)> EFilterFunc;

	// Class for elastic coefficient order parameter of liquid crystals, based on the 
	// Frank-Oseen elastic free energy. 
	class ElasticCoeffOP : public DOSOrderParameter, public ParticleObserver
	{
		private:
			arma::mat _Q;
			std::map<int, arma::vec> _idmap;
			arma::vec _tmpVec;
			EFilterFunc _efunc;
			arma::cx_vec _eigval;
			arma::cx_mat _eigvec;
			arma::uword _imax;
			
			// Particle count for averaging.
			int _pcount;

			// Delta distance for elastic coefficient calculation.
			double _dxj;

			// System temperature (reduced units).
			double _temperature;

		public:

			// Initialize ElasticCoeffOP class. A user supplied filter function will determine which 
			// particles will contribute to the elastic order parameter (true for include, false otherwise).
			// The value h represents the length over which to compute the derivative (dni/dxj).
			ElasticCoeffOP(const World& world, double temperature, double dxj, EFilterFunc efunc) : 
				_Q(3,3,arma::fill::zeros), _idmap(), _tmpVec(3, arma::fill::zeros), 
				_efunc(efunc), _eigval(3, arma::fill::zeros), _eigvec(3,3,arma::fill::zeros), 
				_imax(0), _pcount(0), _dxj(dxj), _temperature(temperature)
			{

				for (int i = 0; i < world.GetParticleCount(); ++i)
				{
					auto* p = world.SelectParticle(i);

					// Particle is counted.
					if(_efunc(p))
					{
						++_pcount;

						int id = p->GetGlobalIdentifier();
						auto& dir = p->GetDirectorRef();

						_tmpVec = dir;
						_Q += arma::kron(_tmpVec.t(), _tmpVec) - 1.0/3.0*arma::eye(3,3);
						_idmap.insert(std::pair<int, arma::vec>(id, _tmpVec));
					}
				}

				// Average.
				_Q *= 3.0/(2.0*_pcount);
			}


			// Evaluate the order parameter.
			inline virtual double EvaluateParameter(double) override
			{
				double dny = _eigvec(1, _imax).real();
				
				// Return dny/dx. (this is twist, hardcoded for now). 
				return dny/_dxj;

			}

			inline double AcceptanceProbability(double prevE, double prevO, 
												double newE, double newO) override
			{
				double p = exp(-(newE - prevE)/_temperature + prevO - newO);
				return p > 1.0 ? 1.0 : p;
			}

			// Update Q tensor on particle director change.
			virtual void Update(const ParticleEvent& pEvent) override
			{
				if(!pEvent.director)
					return;

				auto* p = pEvent.GetParticle();
				if(!_efunc(p))
					return;

				int id = p->GetGlobalIdentifier();
				auto& dir = p->GetDirectorRef();
				arma::vec& prevDir = _idmap[id];

				_tmpVec = dir;
				_Q += 3.0/(2.0*_pcount)*(arma::kron(_tmpVec.t(), _tmpVec) - arma::kron(prevDir.t(), prevDir));
				prevDir = _tmpVec;

				// Eager decomposition. Only on appropriate update.
				if(!arma::eig_gen(_eigval, _eigvec, _Q))
				   std::cerr << "Eigenvalue decomposition failed!!" << std::endl;

				_eigval.max(_imax);
			}

			// Get temperature.
			virtual double GetTemperature() override
			{
				return _temperature;
			}

			std::vector<double> GetDirector()
			{
				return {_eigvec(0, _imax).real(), _eigvec(1, _imax).real(), _eigvec(2, _imax).real()};
			}

	};
}