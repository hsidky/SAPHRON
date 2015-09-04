#pragma once 

namespace SAPHRON
{
	// Class that hold useful information to be used across SAPHRON
	// such as physical constants. 
	class SimInfo
	{
	private:
		double _kb = 1.0;
	public:

		// Get singleton (I know, I know...) instance of 
		// SimInfo.
		static SimInfo& Instance() 
		{
    		static SimInfo instance;
    		return instance;
		}

		// Get/set Boltzmann constant.
		double GetkB() { return _kb; }
		void SetkB(double kb) { _kb = kb; }
	};
}