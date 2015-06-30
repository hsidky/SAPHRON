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
		double connectivity;
		
		Energy() : inter(0.0), bonded(0.0), connectivity(0.0){}

		Energy(double interE, double intraE, double connectivityE, double bondedE) : 
			inter(interE), intra(intraE), bonded(bondedE), connectivity(connectivityE){}
		
		double total()
		{
			return inter + intra + connectivity + bonded;
		}

		inline bool operator==(const Energy& rhs) const
		{
			return inter == rhs.inter && intra == rhs.intra && connectivity == rhs.connectivity && bonded == rhs.bonded;
		}

		inline bool operator!=(const Energy& rhs) const
		{
			return inter != rhs.inter || intra != rhs.intra || connectivity != rhs.connectivity || bonded != rhs.bonded;
		}

		inline Energy& operator+=(const Energy& rhs)
		{
			inter += rhs.inter; 
			intra += rhs.intra; 
			connectivity += rhs.connectivity;
			bonded += rhs.bonded;
			return *this;
		}

		inline Energy& operator-=(const Energy& rhs)
		{
			inter -= rhs.inter; 
			intra -= rhs.intra; 
			connectivity -= rhs.connectivity;
			bonded -= rhs.bonded;
			return *this;
		}

		inline Energy& operator/=(const double& rhs)
		{
			inter /= rhs; 
			intra /= rhs; 
			bonded /= rhs; 
			connectivity /= rhs;
			return *this;
		}
	};

	inline Energy operator+(Energy lhs, const Energy& rhs)
	{
		lhs.inter += rhs.inter;
		lhs.intra += rhs.intra;
		lhs.bonded += rhs.bonded;
		lhs.connectivity += rhs.connectivity;
		return lhs;
	}

	inline Energy operator-(Energy lhs, const Energy& rhs)
	{
		lhs.inter -= rhs.inter;
		lhs.intra -= rhs.intra;
		lhs.bonded -= rhs.bonded;
		lhs.connectivity -= rhs.connectivity;
		return lhs;
	}

	inline Energy operator/(Energy lhs, const double& rhs)
	{
		lhs.inter /= rhs;
		lhs.intra /= rhs;
		lhs.bonded /= rhs;
		lhs.connectivity /= rhs;
		return lhs;
	}
}