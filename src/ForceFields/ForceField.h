#pragma once

#include "../Particles/Particle.h"
#include "../Properties/Energy.h"
#include "../Properties/Pressure.h"


namespace SAPHRON
{
	// Energy pressure tuple.
	struct EPTuple
	{
		Energy energy;
		Pressure pressure;

		inline EPTuple& operator+=(const EPTuple& rhs)
		{
			energy += rhs.energy;
			pressure += rhs.pressure;
			return *this;
		}

		inline EPTuple& operator-=(const EPTuple& rhs)
		{
			energy -= rhs.energy;
			pressure -= rhs.pressure;
			return *this;
		}		
	};

	inline EPTuple operator+(EPTuple lhs, const EPTuple& rhs)
	{
		lhs.energy += rhs.energy;
		lhs.pressure += rhs.pressure;
		return lhs;
	}

	inline EPTuple operator-(EPTuple lhs, const EPTuple& rhs)
	{
		lhs.energy -= rhs.energy;
		lhs.pressure -= rhs.pressure;
		return lhs;
	}

	// Energy-virial interaction structure.
	struct Interaction
	{
		double energy;
		double virial;

		Interaction() : energy(0.0), virial(0.0) {}
		Interaction(double e, double v) : energy(e), virial(v) {}

		inline Interaction& operator=(const Interaction& rhs)
		{
			energy = rhs.energy;
			virial = rhs.virial;
			return *this;
		}

		inline Interaction& operator+=(const Interaction& rhs)
		{
			energy += rhs.energy;
			virial += rhs.virial;
			return *this;
		}

		inline Interaction& operator-=(const Interaction& rhs)
		{
			energy -= rhs.energy;
			virial -= rhs.virial;
			return *this;
		}
	};

	inline Interaction operator+(Interaction lhs, const Interaction& rhs)
	{
		lhs.energy += rhs.energy;
		lhs.virial += rhs.virial;
		return lhs;
	}

	inline Interaction operator-(Interaction lhs, const Interaction& rhs)
	{
		lhs.energy -= rhs.energy;
		lhs.virial -= rhs.virial;
		return lhs;
	}

	// Abstract base class for a force field. Represents the scalar interaction potential between
	// two bodies (particles). It calculates energy and intermolecular virial contribution. 
	// The virial contribution is written as in Allan & Tildesley Eq. 2.60 and 2.63.
	// The forcefield implementation should return w(r)/r^2.
	class ForceField
	{
		public:
			// Returns the potential and virial contribution between two particle. The distance vector 
			// between the two particles is provided by the FFManager with minimum image applied.
			virtual Interaction Evaluate(const Particle& p1, const Particle& p2, const Position& rij) = 0;

			// Evaluates the energy tail correction term. This is precisely integral(u(r)*r^2,rc,inf). The 
			// remainder is taken care of by the forcefield manager.
			virtual double EnergyTailCorrection() { return 0.0; }

			// Evalutes the pressure tail correction term.
			virtual double PressureTailCorrection() { return 0.0; }

			virtual ~ForceField() {}
	};
}
