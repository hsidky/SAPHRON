#pragma once 

#include "NonBondedFF.h"
#include <math.h>

namespace SAPHRON
{
	// Classic anisotropic Gay-Berne potential for interaction between
	// two uniaxial aspherical particles. Expressions and notation obtained from:
	// Golubkov & Ren, J. Chem. Phys. 125, 064103 (2006).
	class GayBerneFF : public NonBondedFF
	{
	private:
		// Commonly used parameters.
		double sig0_, X_, asq_, Xasq_, Xa_sq_, Xsq_, Xpsq_, Xpapsq_, Xpap_sq_;

		// Particle diameters and lengths.
		double di_, dj_, li_, lj_;

		// Shape parameters.
		double mu_, nu_;

		// Particle interaction energies.
		// eps0 = scaling, epsE = end-to-end/face-to-face,
		// epsS = side-to-side.
		double eps0_, epsE_, epsS_;

		// Potential softness.
		double dw_;

		// Cutoff radii.
		CutoffList rc_;

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
		sig0_(0), X_(0), Xasq_(0), Xa_sq_(0), Xsq_(0), Xpsq_(0), Xpapsq_(0), Xpap_sq_(0),
		di_(di), dj_(dj), li_(li), lj_(lj), mu_(mu), nu_(nu), eps0_(eps0), 
		epsE_(epsE), epsS_(epsS), dw_(dw), rc_(rc)
		{
			using std::pow;

			// Compute commonly used parameters.
			sig0_ = 0.5*(di + dj);
			X_ = sqrt(((li*li-di*di)*(lj*lj-dj*dj))/((lj*lj+di*di)*(li*li+dj*dj)));
			asq_ = sqrt(((li*li-di*di)*(lj*lj+di*di))/((lj*lj-dj*dj)*(li*li+dj*dj)));
			Xasq_ = (li*li-di*di)/(li*li+dj*dj);
			Xa_sq_ = (lj*lj-dj*dj)/(lj*lj+di*di);
			Xsq_ = X_*X_;
			Xpsq_ = pow((1-pow(epsE/epsS, 1./mu))/(1+pow(epsE/epsS, 1./mu)),2.);
			Xpapsq_ = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
			Xpap_sq_ = (pow(epsS, 1./mu)-pow(epsE, 1./mu))/(pow(epsS, 1./mu)+pow(epsE, 1./mu));
		}

		double EvaluateEnergy(
			const Site& s1, 
			const Site& s2, 
			const Vector3& rij, 
			double rsq,
			uint wid) const override
		{
			double u = 0.;

			auto r = sqrt(rsq);
			if(r > rc_[wid])
				return u;
			
			auto& ui = s1.director;
			auto& uj = s2.director;

			auto uirij = ui.dot(rij)/r;
			auto ujrij = uj.dot(rij)/r;
			auto uiuj = ui.dot(uj);

			auto H =  (Xasq_*uirij*uirij   + Xa_sq_*ujrij*ujrij   - 2.*Xsq_*uirij*ujrij*uiuj)/(1.  - Xsq_*uiuj*uiuj);
			auto Hp = (Xpapsq_*uirij*uirij + Xpap_sq_*ujrij*ujrij - 2.*Xpsq_*uirij*ujrij*uiuj)/(1. - Xpsq_*uiuj*uiuj);
			auto sig = sig0_/sqrt(1.0-H);
			auto eps1 = 1.0/sqrt(1.0-Xsq_*uiuj*uiuj);
			auto eps2 = 1.0 - Hp;
			auto eps = eps0_*pow(eps1, nu_)*pow(eps2, mu_);
			auto R = dw_*sig0_/(r - sig + dw_*sig0_);
			u = 4.0*eps*(pow(R, 12.) - pow(R, 6.));

			// A check for unphysicalness. R is an approximation 
			// of the surface to surface distance. It should never be 
			// negative.
			if(R < 0)
				return 1.0e10/r;

			return u;
		}
		/*
		virtual void Serialize(Json::Value& json) const override
		{
			json["type"] = "GayBerne"; 
			json["diameters"][0] = di_;
			json["diameters"][1] = dj_;
			json["lengths"][0] = li_;
			json["lengths"][1] = lj_;
			json["eps0"] = eps0_;
			json["epsE"] = epsE_;
			json["epsS"] = epsS_;
			json["mu"] = mu_;
			json["nu"] = nu_;
			json["dw"] = dw_;
			for(auto& rc : rc_)
				json["rcut"].append(rc);
		}*/

		~GayBerneFF(){}
	};
}
