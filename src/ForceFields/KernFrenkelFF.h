#pragma once 
#include "ForceField.h"
#include "Utils/Helpers.h"

namespace SAPHRON
{
	// Class for Kern-Frenkel multi-patch potential as described in:
	// Kern, N. and Frenkel, D., JCP, 118, 9882-9889 (2003)
	// DOI:http://dx.doi.org/10.1063/1.1569473
	class KernFrenkelFF : public ForceField
	{
	private:
		double _epsilon, _sigma, _delta;
		std::vector<double> _thetas, _costhetas; 
		std::vector<Vector3D> _pjs;
		bool _invert; 
	public: 
		// Creates a new Kern-Frenkel patchy potential. 
		// epislon: Energy scale.
		// sigma: Hard core radius. 
		// delta: Maximum additional interaction range (rmax = sigma + delta).
		// thetas: Vector of opening angles for each patch. 
		// pjs: Vector of unit vectors pointing towards centers of each patch.
		//      Note: these will be automatically normalized.
		// invert: Invert the potential such that the potential is sticky 
		//         in the compliment of the patch.
		KernFrenkelFF(
			double epsilon,
			double sigma, 
			double delta, 
			const std::vector<double>& thetas, 
			const std::vector<Vector3D>& pjs,
			bool invert = false) : 
		_epsilon(epsilon), _sigma(sigma), _delta(delta), 
		_thetas(thetas), _costhetas(thetas.size()), _pjs(pjs),
		_invert(invert)
		{
			// Normalize p vectors.
			for(auto& p : _pjs)
				p /= fnorm(p);

			// Compute cosine thetas.
			for(size_t i = 0; i < thetas.size(); ++i)
				_costhetas[i] = cos(thetas[i]);
		}

		Interaction Evaluate(const Particle& pi, 
							 const Particle& pj, 
							 const Position& rij, 
							 unsigned int) const override
		{
			Interaction ep;

			auto r = fnorm(rij);

			// Hard particle limit.
			if(r < _sigma)
				return {1e7, 0};

			// Square well.
			auto sw = (r < _sigma + _delta) ? -_epsilon : 0;
			if(sw == 0)
				return {sw, 0};

			// Compute required rotation matrices relative to n_z.
			auto nz = Vector3D{0, 0, 1};
			auto Ri = RotationMatrixFromVecs(nz, pi.GetDirector());
			auto Rj = RotationMatrixFromVecs(nz, pj.GetDirector());

			// Angular term. 
			auto phi = (_invert) ? 1.0 : 0.0;
			for(size_t i = 0; i < _costhetas.size(); ++i)
			{
				for(size_t j = 0; j < _costhetas.size(); ++j)
				{
					Vector3D ui = Ri*_pjs[i];
					Vector3D uj = Rj*_pjs[j];
					if(_invert)
					{
						if(fdot(ui,rij/r) > _costhetas[i] &&
						   fdot(uj,-rij/r) > _costhetas[j])
						{
							phi = 0.0;
							return {sw*phi, 0};
						}	
					}
					else
					{
						if(fdot(ui,rij/r) > _costhetas[i] && 
						   fdot(uj,-rij/r) > _costhetas[j])
						{
							phi = 1.0;
							return {sw*phi, 0};
						}
					}
					
				}
			}

			return {sw*phi, 0};
		}

		void Serialize(Json::Value& json) const override
		{
			json["type"] = "KernFrenkel";
			json["epsilon"] = _epsilon;
			json["sigma"] = _sigma;
			json["delta"] = _delta;
			for(auto& t : _thetas)
				json["thetas"].append(t);
			for(auto& u : _pjs)
			{
				Json::Value arr;
				arr[0] = u[0];
				arr[1] = u[1];
				arr[2] = u[2];
				json["pjs"].append(arr);
			}
			json["invert"] = _invert;
		}
	};
}