#pragma once 

#include "ForceField.h"
#include <math.h>

namespace SAPHRON
{
	// Classic anisotropic Gay-Berne potential for interaction between
	// two uniaxial aspherical particles. Expressions and notation obtained from:
	// Golubkov & Ren, J. Chem. Phys. 125, 064103 (2006).
	class GayBerneFF : public ForceField
	{
	private:
		// Commonly used parameters.
		double _sig0, _X, _asq, _Xasq, _Xa_sq, _Xsq, _Xpsq, _Xpapsq, _Xpap_sq;

		// Particle diameters and lengths.
		double _di, _dj, _li, _lj;

		// Shape parameters.
		double _mu, _nu;

		// Particle interaction energies.
		// eps0 = scaling, epsE = end-to-end/face-to-face,
		// epsS = side-to-side.
		double _eps0, _epsE, _epsS;

		// Potential softness.
		double _dw;

		// Cutoff radii.
		CutoffList _rc;

	public:
		// Initialize Gay-Berne potential between two uniaxial aspherical particles. 
		// di, dj: particle diameters.
		// li, lj: particle lengths.
		// eps0, epsE, epsS: cross interaction, end-to-end, side-to-side.
		// dw : potential softness.
		// rc: List of cutoff radii for world(s).
		// mu, nu: Shape parameters. Defaults are 2, 1.
		GayBerneFF(
			double di, double dj, double li, double lj, 
			double eps0, double epsE, double epsS, double dw,
			const CutoffList& rc, double mu = 2.0, double nu = 1.0) : 
		_sig0(0), _X(0), _Xasq(0), _Xa_sq(0), _Xsq(0), _Xpsq(0), _Xpapsq(0), _Xpap_sq(0),
		_di(di), _dj(dj), _li(li), _lj(lj), _mu(mu), _nu(nu), _eps0(eps0), 
		_epsE(epsE), _epsS(epsS), _dw(dw), _rc(rc)
		{
			using std::pow;

			// Compute commonly used parameters.
			_sig0 = 0.5*(di + dj);
			_X = sqrt(((li*li-di*di)*(lj*lj-dj*dj))/((lj*lj+di*di)*(li*li+dj*dj)));
			_asq = sqrt(((li*li-di*di)*(lj*lj+di*di))/((lj*lj-dj*dj)*(li*li+dj*dj)));
			_Xasq = (li*li-di*di)/(li*li+dj*dj);
			_Xa_sq = (lj*lj-dj*dj)/(lj*lj+di*di);
			_Xsq = _X*_X;
			_Xpsq = pow((1-pow(epsE/epsS, 1./mu))/(1+pow(epsE/epsS, 1./mu)),2.);
			_Xpapsq = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
			_Xpap_sq = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
		}

		virtual Interaction Evaluate(
			const Particle& p1, 
			const Particle& p2, 
			const Position& rij, 
			unsigned int wid) override
		{
			Interaction ep;

			auto r = fnorm(rij);
			if(r > _rc[wid])
				return ep;

			auto& ui = p1.GetDirector();
			auto& uj = p2.GetDirector();

			auto uirij = fdot(ui, rij)/r;
			auto ujrij = fdot(uj, rij)/r;
			auto uiuj = fdot(ui, uj);

			auto H =  (_Xasq*uirij*uirij   + _Xa_sq*ujrij*ujrij   - 2.*_Xsq*uirij*ujrij*uiuj)/(1.  - _Xsq*uiuj*uiuj);
			auto Hp = (_Xpapsq*uirij*uirij + _Xpap_sq*ujrij*ujrij - 2.*_Xpsq*uirij*ujrij*uiuj)/(1. - _Xpsq*uiuj*uiuj);
			auto sig = _sig0/sqrt(1.0-H);
			auto eps1 = 1.0/sqrt(1.0-_Xsq*uiuj*uiuj);
			auto eps2 = 1.0 - Hp;
			auto eps = _eps0*pow(eps1, _nu)*pow(eps2, _mu);
			auto R = _dw*_sig0/(r - sig + _dw*_sig0);
			ep.energy = 4.0*eps*(pow(R, 12.) - pow(R, 6.));

			return ep;
		}

		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "GayBerne"; 
			json["diameters"][0] = _di;
			json["diameters"][1] = _dj;
			json["lengths"][0] = _li;
			json["lengths"][0] = _lj;
			json["eps0"] = _eps0;
			json["epsE"] = _epsE;
			json["epsS"] = _epsS;
			json["mu"] = _mu;
			json["nu"] = _nu;
			for(auto& rc : _rc)
				json["rcut"].append(rc);
		}

		~GayBerneFF(){}
	};
}