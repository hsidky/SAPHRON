#pragma once 

namespace SAPHRON
{
	class DOSOrderParameter
	{
		public:
			
			// Return the order parameter based on the energy.
			virtual double EvaluateParameter(double energy) = 0;

			// Evaluate the acceptance probability based on previous enerergy (prevE), previous 
			// value of Log(\Omega) (prevO) and new values.
			virtual double AcceptanceProbability(double prevE, double prevO, double newE, double newO) = 0;

			virtual ~DOSOrderParameter(){}
	};
} 	