#pragma once 

#include <armadillo>
#include <functional>
#include <map>

#include "DOSOrderParameter.h"
#include "../Particles/ParticleObserver.h"
#include "../Worlds/World.h"
#include "../Simulation/SimInfo.h"

namespace SAPHRON
{
	using PosFilter =  std::function<bool(const Position&)>;

	enum ElasticMode
	{
		Splay,
		Bend,
		Twist
	};

	// Class for elastic coefficient order parameter of liquid crystals, based on the 
	// Frank-Oseen elastic free energy. 
	class ElasticCoeffOP : public DOSOrderParameter, public ParticleObserver
	{
	private:
		Matrix3D _Q;
		PosFilter _efunc;
		arma::cx_colvec3 _eigval;
		arma::cx_mat33 _eigvec;
		arma::uword _imax;
		
		// Particle count for averaging.
		int _pcount;

		// Delta distance for elastic coefficient calculation.
		double _dxj;

		// World ID. 
		int _wid;

		// x-range for filter function.
		std::array<double, 2> _xrange;

		// Elastic mode. 
		ElasticMode _mode;

		// Update Q tensor by performing eigen decomposition.
		void UpdateQTensor()
		{
			if(!arma::eig_gen(_eigval, _eigvec, _Q))
			   std::cerr << "Eigenvalue decomposition failed!!" << std::endl;

			_eigval.max(_imax);
		}

	protected:
		
		// Calculate acceptance probability.
		virtual double CalcAcceptanceProbability(const Energy& ei, 
												 const Energy& ef, 
												 double opi, 
												 double opf,
												 const World& w) const override
		{
			auto& sim = SimInfo::Instance();
			auto de = ef.total() - ei.total();
			double p = exp(-de/(sim.GetkB()*w.GetTemperature()) + GetHistValue(opi) - GetHistValue(opf));
			return p > 1.0 ? 1.0 : p;
		}

	public:

		// Initialize ElasticCoeffOP class. A user supplied filter function will determine which 
		// particles will contribute to the elastic order parameter (true for include, false otherwise).
		// The value h represents the length over which to compute the derivative (dni/dxj).
		ElasticCoeffOP(const Histogram& hist, World* world, double dxj, std::array<double, 2> xrange, ElasticMode mode) : 
			DOSOrderParameter(hist), _Q(arma::fill::zeros), _efunc(), _eigval(arma::fill::zeros), 
			_eigvec(arma::fill::zeros), _imax(0), _pcount(0), _dxj(dxj), _wid(world->GetID()),
			_xrange(xrange), _mode(mode)
		{

			_efunc = [=](const Position& pos) -> bool {
				return pos[0] >= _xrange[0] && pos[0] <= _xrange[1];
			};

			for (int i = 0; i < world->GetParticleCount(); ++i)
			{
				auto* p = world->SelectParticle(i);
				p->AddObserver(this);

				// Particle is counted.
				if(_efunc(p->GetPosition()))
				{
					++_pcount;
					auto& dir = p->GetDirector();
					_Q += arma::kron(dir.t(), dir) - 1.0/3.0*arma::eye(3,3);
				}
			}

			// Average.
			_Q *= 3.0/(2.0*_pcount);
			UpdateQTensor();
		}


		// Evaluate the order parameter.
		virtual double EvaluateOrderParameter(const World&) const override
		{
			double dni;
			switch(_mode)
			{
				case Splay:
					dni = _eigvec(0, _imax).real();
					break;
				case Twist:
					dni = _eigvec(1, _imax).real();
					break;
				case Bend:
					dni = _eigvec(0, _imax).real();
			}

			// We expect the eigenvector to point in the first two quadrants. 
			// Flip sign of dni if it's not. This is to prevent fluctuations of the 
			// order parameter from one side to the other. 
			dni = (_eigvec(2, _imax).real() < 0) ? -dni : dni;

			// Return dni/dx. (this is twist, hardcoded for now). 
			return dni/_dxj;
		}

		// Update Q tensor on particle director change.
		virtual void ParticleUpdate(const ParticleEvent& pEvent) override
		{
			// Get particle and positions, directors.
			auto* p = pEvent.GetParticle();
			auto& pos = p->GetPosition();
			auto& dir = p->GetDirector();
			
			// If only director has changed, check if it's in the region
			// and update.
			if(pEvent.director && _efunc(pos))
			{
				auto& pdir = pEvent.GetOldDirector();
				_Q += 3.0/(2.0*_pcount)*(arma::kron(dir.t(), dir) - arma::kron(pdir.t(), pdir));
				UpdateQTensor();
				return;
			}
			else if(pEvent.position)
			{
				auto& ppos = pEvent.GetOldPosition();
				// Three possible cases on a position change:
				// 1. Particle previously not in region but now in region. 
				// 2. Particle previously in region and still in region 
				//    (we don't do anything since nothing's changed).
				// 3. Particle previously in region but now not in region.
				if(!_efunc(ppos) && _efunc(pos))
				{
					// Update normalization.
					_Q *= _pcount/(_pcount + 1.);
					++_pcount;

					_Q += 3.0/(2.0*_pcount)*(arma::kron(dir.t(), dir) - 1.0/3.0*arma::eye(3,3));
					UpdateQTensor();
				}
				else if(_efunc(ppos) && !_efunc(pos))
				{
					_Q *= _pcount/(_pcount - 1.);
					--_pcount;

					_Q -= 3.0/(2.0*_pcount)*(arma::kron(dir.t(), dir) - 1.0/3.0*arma::eye(3,3));
					UpdateQTensor();
				}
			}
		}

		// Serialize.
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "ElasticCoeff";
			
			switch(_mode)
			{
				case Splay:
					json["mode"] = "splay";
					break;
				case Twist:
					json["mode"] = "twist";
					break;
				case Bend:
					json["mode"] = "bend";
			}
			
			json["world"] = _wid;
			json["xrange"][0] = _xrange[0];
			json["xrange"][1] = _xrange[1];
		}

		// Get layer director.
		Director GetDirector()
		{
			return arma::real(_eigvec.col(_imax));
		}

	};
}
