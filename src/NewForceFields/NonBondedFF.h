#pragma once 

#include "../Particles/NewParticle.h"

namespace SAPHRON
{
	// Forward declare.
	class NonBondedFF;

	// Typdefs. 
	using NBFFList = std::vector<NonBondedFF*>;
	using CutoffList = std::vector<double>;

	// Abstract base class for a nonbonded forcefield. Represents the scalar interaction 
	// potential between two bodies (sites). 
	class NonBondedFF 
	{
	public:
		// Returns the potential energy between two sites. The distance vector and 
		// norm squared between the two particles is provided by the FFManager with minimum 
		// image applied, in addition to the integer world ID if applicable, zero if there 
		// is none. 
		virtual double EvaluateEnergy(
			const Site& s1, 
			const Site& s2, 
			const Vector3& rij, 
			double rsq, 
			uint wid) const = 0;

		// Returns the force between two sites. The distance vector and norm squared 
		// between the two particles is provided by the FFManager with minimum image 
		// applied, in addition to the integer world ID if applicable. Zero if there is 
		// none.
		virtual Vector3 EvaluateForce(
			const Site&, 
			const Site&, 
			const Vector3&, 
			double, 
			uint) const 
		{ 
			return {0., 0., 0.}; 
		};

		// Evaluates the energy tail correction term. 
		// This is precisely integral(u(r)*r^2,rc,Inf).
		// The remainder is taken care of by the forcefield manager. 
		// The world ID (or 0) is passed in.
		virtual double EnergyTailCorrection(uint) const { return 0.; }

		// Evaluates the pressure tail correction term. 
		// The world ID (or 0) is passed in.
		virtual double PressureTailCorrection(uint) const { return 0.; }
	};
}