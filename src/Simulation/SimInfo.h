#pragma once 

namespace SAPHRON
{
	// Class that hold useful information to be used across SAPHRON
	// such as physical constants. 
	class SimInfo
	{
	private:
		// Boltzmann constant.
		double _kb = 1.0;

		// Boltzmann constant (J/K).
		double _KB = 1.3806488E-23;

		// Permittivity of vacuum C2/(J angstrom).
		double _vacperm = 8.854187817E-22;

		// Elementary charge (C).
		double _elem = 1.602176565E-19;
	public:

		// Get singleton (I know, I know...) instance of 
		// SimInfo.
		static SimInfo& Instance() 
		{
    		static SimInfo instance;
    		return instance;
		}

		// Get Boltzmann constant in default units (J/K).
		double GetkBDefault() const { return _KB; }
		
		// Get/set Boltzmann constant.
		double GetkB() const { return _kb; }
		void SetkB(double kb) { _kb = kb; }

		// Get/set vacuum permittivity.
		double GetVacPerm() const { return _vacperm; }
		void SetVacPerm(double eps) { _vacperm = eps; }

		// Get/set elementary charge.
		double GetElemCharge() const { return _elem; }
		void SetElemeCharge(double e) { _elem = e; }
	};
}