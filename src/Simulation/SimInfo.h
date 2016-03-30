#pragma once 

#include "../Utils/Timer.h"

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
		SimUnits _units;
		Timer _timer;

		// Name map for timer names.
		std::map<std::string, std::string> _namemap = {
			{"nlist", "Neighbor list generation"},
			{"total", "Total"},
			{"e_inter", "Intermolecular energy"},
			{"e_intra", "Intramolecular energy"}
		};

		/***************************
		 *    These are defaults   * 
		 ***************************/

		// Boltzmann constant (J/mol*K).
		double _kb = 8.314459848;

		// Boltzmann constant (amu*angstrom^2/ps^2-K)
		double _kbamu = 0.8314459848;

		// Converts charge potential to (J/mol).
		double _charge = 1.389354578390845E+06; 

		// Permittivity of vacuum C^2/(J/mol-angstrom).
		double _vacperm = 5.332117004217066E+02;

		// Elementary charge (C).
		double _elem = 1.602176565E-19;

		// Avogadro's constant (atoms/mol).
		double _na = 6.02214129E+23;

		// Pressure conversion to bar from J*atom/(mol*angstrom^3).
		double _pconv = 1.0e30/(6.02214129E+23*1.0e5);

		// Planck constant (amu*angstrom^2/ps).
		double _h = 39.9031268605;

		/********************************
		 * These are what the user gets * 
		 ********************************/
		double _echarge = 1.0;
		double _ekb = 1.0;
		double _ekbamu = 1.0;
		double _epconv = 1.0;

		// Private constructor. 
		SimInfo(const SimInfo&);

	public:
		SimInfo() : _timer() {}

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
				_epconv = _pconv;
				_units = real;
			}
			else
			{
				_echarge = 1.0;
				_ekb = 1.0;
				_ekbamu = _kbamu;
				_units = reduced;
				_epconv = 1.0;
			}
		}

		// Start a timer.
		void StartTimer(const std::string& name) { _timer.PressStart(name); }

		// Add time to timer. 
		void AddTime(const std::string& name) { _timer.AddTime(name); }

		const TimerMap& GetTimerMap() const { return _timer.GetTimerMap(); }

		std::string ResolveTimerName(const std::string& name) const
		{
			if(_namemap.find(name) == _namemap.end())
				return name;
			else
				return _namemap.at(name);
		}

		// Get sim units.
		SimUnits GetUnits() const  { return _units; }

		// Get Avogadro's constant.
		double GetNa() const { return _na; }
		
		// Get Boltzmann constant.
		double GetkB() const { return _ekb; }

		// Get Boltzmann constant in amu.
		double GetkBamu() const { return _ekbamu; }

		// Get charge conversion factor.
		double GetChargeConv() const { return _echarge; }

		// Get/set vacuum permittivity.
		double GetVacPerm() const { return _vacperm; }

		// Get/set elementary charge.
		double GetElemCharge() const { return _elem; }

		// Get pressure conversion factor (bar).
		double GetPressureConv() const { return _epconv; };

		// Get Planck constant.
		double GetPlanck() const { return _h; }
	};
}