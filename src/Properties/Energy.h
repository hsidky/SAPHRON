#pragma once 

namespace SAPHRON
{
	// Simple POD struct for energy. Just a holder and is not part of the Property 
	// interface.
	struct Energy
	{
		double inter;
		double intra;
		double bonded;
		double electrostatic;
		double connectivity;
		
		Energy() : inter(0.0), bonded(0.0), electrostatic(0.0), connectivity(0.0){}

		Energy(double interE, double intraE, double bondedE, double electrostaticE, double connectivityE ) : 
			inter(interE), intra(intraE), bonded(bondedE), electrostatic(electrostaticE), connectivity(connectivityE){}
		
		double total()
		{
			return inter + intra + electrostatic + connectivity + bonded;
		}

		inline bool operator==(const Energy& rhs) const
		{
			return inter == rhs.inter && intra == rhs.intra && electrostatic == rhs.electrostatic && 
			connectivity == rhs.connectivity && bonded == rhs.bonded;
		}

		inline bool operator!=(const Energy& rhs) const
		{
			return inter != rhs.inter || intra != rhs.intra || electrostatic != rhs.electrostatic || 
			connectivity != rhs.connectivity || bonded != rhs.bonded;
		}

		inline Energy& operator+=(const Energy& rhs)
		{
			inter += rhs.inter; 
			intra += rhs.intra; 
			connectivity += rhs.connectivity;
			bonded += rhs.bonded;
			electrostatic += rhs.electrostatic;
			return *this;
		}

		inline Energy& operator-=(const Energy& rhs)
		{
			inter -= rhs.inter; 
			intra -= rhs.intra; 
			connectivity -= rhs.connectivity;
			bonded -= rhs.bonded;
			electrostatic -= rhs.electrostatic;
			return *this;
		}

		inline Energy& operator/=(const double& rhs)
		{
			inter /= rhs; 
			intra /= rhs; 
			bonded /= rhs; 
			electrostatic /= rhs; 
			connectivity /= rhs;
			return *this;
		}
	};

	inline Energy operator+(Energy lhs, const Energy& rhs)
	{
		lhs.inter += rhs.inter;
		lhs.intra += rhs.intra;
		lhs.bonded += rhs.bonded;
		lhs.electrostatic += rhs.electrostatic;
		lhs.connectivity += rhs.connectivity;
		return lhs;
	}

	inline Energy operator-(Energy lhs, const Energy& rhs)
	{
		lhs.inter -= rhs.inter;
		lhs.intra -= rhs.intra;
		lhs.bonded -= rhs.bonded;
		lhs.electrostatic -= rhs.electrostatic;
		lhs.connectivity -= rhs.connectivity;
		return lhs;
	}

	inline Energy operator/(Energy lhs, const double& rhs)
	{
		lhs.inter /= rhs;
		lhs.intra /= rhs;
		lhs.bonded /= rhs;
		lhs.electrostatic /= rhs;
		lhs.connectivity /= rhs;
		return lhs;
	}
}