#pragma once 

namespace SAPHRON
{
	// Simple POD struct for energy. Just a holder and is not part of the Property 
	// interface.
	struct Energy
	{
		double intervdw;
		double intravdw;
		double interelectrostatic;
		double intraelectrostatic;
		double bonded;
		double connectivity;
		
		Energy() : intervdw(0.0), intravdw(0.0), interelectrostatic(0.0), intraelectrostatic(0.0), 
		bonded(0.0), connectivity(0.0){}

		Energy(double intervdwE, double intravdwE, double interelectroE, double intraelectroE,
		 double bondedE, double connectivityE ) : 
			intervdw(intervdwE), intravdw(intravdwE), interelectrostatic(interelectroE), intraelectrostatic(intraelectroE),
			bonded(bondedE), connectivity(connectivityE){}
		
		double total() const
		{
			return intervdw + intravdw + interelectrostatic + intraelectrostatic + connectivity + bonded;
		}

		inline bool operator==(const Energy& rhs) const
		{
			return intervdw == rhs.intervdw && intravdw == rhs.intravdw && interelectrostatic == rhs.interelectrostatic && 
			intraelectrostatic == rhs.intraelectrostatic && connectivity == rhs.connectivity && bonded == rhs.bonded;
		}

		inline bool operator!=(const Energy& rhs) const
		{
			return intervdw != rhs.intervdw || intravdw != rhs.intravdw || interelectrostatic != rhs.interelectrostatic || 
			intraelectrostatic != rhs.intraelectrostatic || connectivity != rhs.connectivity || bonded != rhs.bonded;
		}

		inline Energy& operator+=(const Energy& rhs)
		{
			intervdw += rhs.intervdw; 
			intravdw += rhs.intravdw; 
			connectivity += rhs.connectivity;
			bonded += rhs.bonded;
			interelectrostatic += rhs.interelectrostatic;
			intraelectrostatic += rhs.intraelectrostatic;
			return *this;
		}

		inline Energy& operator-=(const Energy& rhs)
		{
			intervdw -= rhs.intervdw; 
			intravdw -= rhs.intravdw; 
			connectivity -= rhs.connectivity;
			bonded -= rhs.bonded;
			interelectrostatic -= rhs.interelectrostatic;
			intraelectrostatic -= rhs.intraelectrostatic;
			return *this;
		}

		inline Energy& operator/=(const double& rhs)
		{
			intervdw /= rhs; 
			intravdw /= rhs; 
			bonded /= rhs; 
			interelectrostatic /= rhs; 
			intraelectrostatic /= rhs; 
			connectivity /= rhs;
			return *this;
		}

		inline Energy operator*(const double& rhs) const
		{
			Energy lhs(*this);
			lhs.intervdw *= rhs; 
			lhs.intravdw *= rhs; 
			lhs.bonded *= rhs; 
			lhs.interelectrostatic *= rhs; 
			lhs.intraelectrostatic *= rhs; 
			lhs.connectivity *= rhs;
			return lhs;
		}
	};

	inline Energy operator+(Energy lhs, const Energy& rhs)
	{
		lhs.intervdw += rhs.intervdw;
		lhs.intravdw += rhs.intravdw;
		lhs.bonded += rhs.bonded;
		lhs.interelectrostatic += rhs.interelectrostatic;
		lhs.intraelectrostatic += rhs.intraelectrostatic;
		lhs.connectivity += rhs.connectivity;
		return lhs;
	}

	inline Energy operator-(Energy lhs, const Energy& rhs)
	{
		lhs.intervdw -= rhs.intervdw;
		lhs.intravdw -= rhs.intravdw;
		lhs.bonded -= rhs.bonded;
		lhs.interelectrostatic -= rhs.interelectrostatic;
		lhs.intraelectrostatic -= rhs.intraelectrostatic;
		lhs.connectivity -= rhs.connectivity;
		return lhs;
	}

	inline Energy operator/(Energy lhs, const double& rhs)
	{
		lhs.intervdw /= rhs;
		lhs.intravdw /= rhs;
		lhs.bonded /= rhs;
		lhs.interelectrostatic /= rhs;
		lhs.intraelectrostatic /= rhs;
		lhs.connectivity /= rhs;
		return lhs;
	}	

	inline Energy operator*(double lhs, const Energy& rhs)
	{
		return rhs*lhs;
	}
}