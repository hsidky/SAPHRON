#pragma once 

namespace SAPHRON
{
	// Class that hold useful information to be used across SAPHRON
	// such as physical constants. 

	enum SimUnits
	{
		reduced = 0,
		real = 1
	};

	class SimInfo
	{
	private:
		/***************************
		 *    These are defaults   * 
		 ***************************/

		// Boltzmann constant (J/mol*K).
		double _kb = 8.314459848;

		// Converts charge potential to (J/mol).
		double _charge = 1.389354578390845E+06; 

		// Permittivity of vacuum C^2/(J/mol-angstrom).
		double _vacperm = 5.332117004217066E+02;

		// Elementary charge (C).
		double _elem = 1.602176565E-19;

		// Avogadro's constant (atoms/mol).
		double _na = 6.02214129E+23;

		/********************************
		 * These are what the user gets * 
		 ********************************/
		double _echarge = 1.0;
		double _ekb = 1.0;

	public:

		// Get singleton (I know, I know...) instance of 
		// SimInfo.
		static SimInfo& Instance() 
		{
    		static SimInfo instance;
    		return instance;
		}

		// Set units.
		void SetUnits(const SimUnits& units) 
		{ 
			if(units == real)
			{
				_echarge = _charge;
				_ekb = _kb;
			}
			else
			{
				_echarge = 1.0;
				_ekb = 1.0;
			}
		}

		// Get Avogadro's constant.
		double GetNa() const { return _na; }
		
		// Get/set Boltzmann constant.
		double GetkB() const { return _ekb; }

		// Get charge conversion factor.
		double GetChargeConv() const { return _echarge; }

		// Get/set vacuum permittivity.
		double GetVacPerm() const { return _vacperm; }

		// Get/set elementary charge.
		double GetElemCharge() const { return _elem; }
	};
}