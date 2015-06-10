#pragma once 

namespace SAPHRON
{
	// Simple POD struct for energy. Just a holder and is not part of the Property 
	// interface.
	struct Energy
	{
		double nonbonded;
		double connectivity;
		
		Energy() : nonbonded(0.0), connectivity(0.0){}

		Energy(double nonbondedE, double connectivityE) : 
			nonbonded(nonbondedE), connectivity(connectivityE){}
		
		double total()
		{
			return nonbonded + connectivity;
		}

		inline bool operator==(const Energy& rhs) const
		{
			return nonbonded == rhs.nonbonded && connectivity == rhs.connectivity;
		}

		inline Energy& operator+=(const Energy& rhs)
		{
			nonbonded += rhs.nonbonded; 
			connectivity += rhs.connectivity;
			return *this;
		}

		inline Energy& operator-=(const Energy& rhs)
		{
			nonbonded -= rhs.nonbonded; 
			connectivity -= rhs.connectivity;
			return *this;
		}

		inline Energy& operator/=(const double& rhs)
		{
			nonbonded /= rhs; 
			connectivity /= rhs;
			return *this;
		}
	};

	inline Energy operator+(Energy lhs, const Energy& rhs)
	{
		lhs.nonbonded += rhs.nonbonded;
		lhs.connectivity += rhs.connectivity;
		return lhs;
	}

	inline Energy operator-(Energy lhs, const Energy& rhs)
	{
		lhs.nonbonded -= rhs.nonbonded;
		lhs.connectivity -= rhs.connectivity;
		return lhs;
	}

	inline Energy operator/(Energy lhs, const double& rhs)
	{
		lhs.nonbonded /= rhs;
		lhs.connectivity /= rhs;
		return lhs;
	}
}