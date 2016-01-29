#pragma once 

#include "Energy.h"
#include "Pressure.h"

namespace SAPHRON
{
	// Energy pressure tuple.
	struct EPTuple
	{
		Energy energy;
		Pressure pressure;

		EPTuple() : energy(), pressure(){}
		EPTuple(double intervdw, double intravdw, double interelect, double intraelect, 
				double bonded, double connectivity, double tail,
				double pideal, double pxx, double pxy, 
				double pxz, double pyy, double pyz, double pzz, double ptail) : 
		energy{intervdw, intravdw, interelect, intraelect, bonded, connectivity, tail}, 
		pressure{pideal, pxx, pxy, pxz, pyy, pyz, pzz, ptail}{}

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

		inline EPTuple operator*(const double& rhs) const
		{
			EPTuple lhs(*this);
			lhs.energy *= rhs;
			lhs.pressure *= rhs;
			return lhs;
		}		
	};

	inline EPTuple operator*(double lhs, const EPTuple& rhs)
	{
		return rhs*lhs;
	}

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
}