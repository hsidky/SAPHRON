// #pragma once
//
// #include "DensityOfStatesEnsemble.h"
//
// namespace Ensembles
// {
// 	// Class for Semi-grand Density-of-States (DOS) sampling.
// 	template<typename T>
// 	class SemiGrandDOSEnsemble : public DensityOfStatesEnsemble<T>
// 	{
// 		private:
//
// 			// Reduced temperature (kbT).
// 			double _temperature;
//
// 			// Number of species 1 sites.
// 			int _n1count = 0, _newn1count;
//
// 		protected:
//
// 			// Calculates the total number of a specified species.
// 			int CalculateSpeciesCount(int species = 1);
//
// 		public:
//
// 			// Initializes a Semi-grand DOS sampler for a specified model at a given temperature (kbT).
// 			// The binning will be performed according to the specified minimum
// 			// and maxiumum mole fractions of species 1 in the system. Note that since
// 			// the there is a discrete number of lattice sites, a bincount larger than the
// 			// difference between the site counts will result in a histogram that never flattens.
// 			SemiGrandDOSEnsemble(BaseModel& model,
// 			                     double minN1,
// 			                     double maxN1,
// 			                     int binCount,
// 			                     double temperature);
//
// 			// Initializes a Semi-grand DOS sampler for a specified model at a given temperature (kbT).
// 			// The binning will be performed according to the specified minimum
// 			// and maxiumum mole fractions of species 1 in the system. Note that since
// 			// the there is a discrete number of lattice sites, a bin width smaller than 1 will result in
// 			// a histogram that never flattens.
// 			SemiGrandDOSEnsemble(BaseModel& model,
// 			                     double minN1,
// 			                     double maxN1,
// 			                     double binWidth,
// 			                     double temperature);
//
// 			// Performs one Monte-Carlo iteration. This is precisely one random draw
// 			// from the model (one function call to model->DrawSample()).
// 			void Iterate();
//
// 			// Semi-grand acceptance probability.
// 			double AcceptanceProbability(double prevH, double currH);
//
// 			// Gets the temperature (K).
// 			double GetTemperature()
// 			{
// 				return this->_temperature;
// 			}
//
// 			// Sets the temperature (K).
// 			double SetTemperature(double temperature)
// 			{
// 				return this->_temperature = temperature;
// 			}
//
// 			// Accept visitor to class.
// 			virtual void AcceptVisitor(class Visitor& v)
// 			{
// 				v.Visit(this);
// 				this->model.AcceptVisitor(v);
// 				this->hist.AcceptVisitor(v);
// 			}
//
// 			// Gets the mixture composition (absolute number of each species).
// 			std::vector<int> GetComposition()
// 			{
// 				return {_n1count, this->model.GetSiteCount() - _n1count};
// 			}
// 	};
// }
