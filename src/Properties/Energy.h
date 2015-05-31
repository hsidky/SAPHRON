#pragma once 

namespace SAPHRON
{
	// Simple POD struct for energy. Just a holder and is not part of the Property 
	// interface.
	struct Energy
	{
		Energy(double nonbondedE, double connectivityE) : 
			nonbonded(nonbondedE), connectivity(connectivityE){}
		
		double nonbonded;
		double connectivity;

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
}