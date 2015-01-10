#pragma once

#include "../Rand.h"
#include "../Site.h"
#include <math.h>
#include <vector>

namespace Models
{
	class BaseModel
	{
		private:

			// "Normalized" Boltzmann constant
			double kb = 1.0;

			// Temperature (K) (Sometimes reduced).
			double temperature;

		protected:

			// Vector of sites
			std::vector<Site> Sites;

			// Random number generator.
			Rand rand;

		public:

			// Initializes the base model with a specific number of sites at a
			// given temperature. The random number generator seed is 1 by default.
			BaseModel(int size, double T, int seed = 1)
				: temperature(T), Sites(size, Site(0.0, 0.0, 0.0)), rand(Rand(seed)) {};

			// Reseed the random number generator. This resets the random number
			// generator.
			void ReseedRand(int seed)
			{
				this->rand = Rand(seed);
			}

			// Returns a random probability between 0 and 1 from a uniform
			// distribution.
			double GetRandomUniformProbability()
			{
				return this->rand.doub();
			}

			// Selects a random site and returns the position in the vector.
			int GetRandomSiteIndex()
			{
				return rand.int64() % (this->Sites.size());
			}

			// Selects a random site and returns a pointer to the Site object.
			Site* SelectRandomSite()
			{
				return &Sites[this->GetRandomSiteIndex()];
			}

			// Selects the site at index and returns a pointer to the Site object.
			Site* SelectSite(int index)
			{
				return &Sites[index];
			}

			// Appends an existing site (shallow copy) to the end of Sites vector.
			void AppendSite(Site newSite)
			{
				Sites.push_back(newSite);
			}

			// Add a new "empty" site to the end of the Sites vector.
			void AddSite()
			{
				Sites.push_back(Site(0.0, 0.0, 0.0));
			}

			// Gets the number of sites in the model.
			int GetSiteCount()
			{
				return this->Sites.size();
			}

			// Gets the "normalized" Boltzmann constant (J/K).
			double GetBoltzmannConstant()
			{
				return this->kb;
			}

			// Gets the temperature (K).
			double GetTemperature()
			{
				return this->temperature;
			}

			// Sets the temperature (K).
			double SetTemperature(double T)
			{
				return this->temperature = T;
			}

			// Evaluate the Hamiltonian for a given site index. The input
			// argument does not have to be used if the Hamiltonian is
			// independent of site.
			double EvaluateHamiltonian(int index)
			{
				return this->EvaluateHamiltonian(&Sites[index]);
			};

			// Evaluate the Hamiltonian for a given site pointer. The input
			// argument does not have to be used if the Hamiltonian is
			// independent of site.
			virtual double EvaluateHamiltonian(Site* site) = 0;

			// Evaluate the acceptance probability of a Monte Carlo move based
			// on the previous and current values of the Hamiltonian.
			virtual double AcceptanceProbability(double prevH, double currH) = 0;
	};
}
