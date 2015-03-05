#pragma once

#include "../Rand.h"
#include "../Site.h"
#include "../Visitors/Visitable.h"
#include <cmath>
#include <vector>

namespace Models
{
	class BaseModel : public Visitors::Visitable
	{
		protected:

			// Vector of sites
			std::vector<Site> Sites;

			// Random number generator.
			Rand rand;

		public:

			// Initializes the base model with a specific number of sites at a
			// given temperature. The random number generator seed is 1 by default.
			BaseModel(int size, int seed = 1)
				: Sites(size, Site(0.0, 0.0, 0.0)), rand(seed){};

			// Reseed the random number generator. This resets the random number
			// generator.
			void ReseedRand(int seed)
			{
				this->rand = Rand(seed);
			}

			// Selects a random site and returns the position in the vector.
			int GetRandomSiteIndex()
			{
				return rand.int32() % (this->Sites.size());
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
				return (int)this->Sites.size();
			}

			// Draws a random site. This is an alias of SelectRandomSite for Ensemble
			// classes.
			virtual Site* DrawSample()
			{
				return this->SelectRandomSite();
			}

			// Evaluate the Hamiltonian for a given site index. The input
			// argument does not have to be used if the Hamiltonian is
			// independent of site.
			double EvaluateHamiltonian(int index)
			{
				return this->EvaluateHamiltonian(Sites[index]);
			};

			// Evaluate the Hamiltonian for a given site pointer. The input
			// argument does not have to be used if the Hamiltonian is
			// independent of site.
			virtual double EvaluateHamiltonian(Site& site) = 0;

			// Accept visitor.
			virtual void AcceptVisitor(class Visitors::Visitor &v)
			{
				v.Visit(this);
				for(auto &site : Sites)
					site.AcceptVisitor(v);
			}

			// Configure a mixture with a certian number of species and
			// targtet mole fractions. The species will be randomly distributed throughout
			// the lattice and the specified mole fractions will only be met approximately
			// since it may not be possible to get a perfect match.
			virtual void ConfigureMixture(int speciesCount, std::vector<double> moleFractions)
			{
				if(moleFractions.size() != (unsigned int) speciesCount)
				{
					std::cerr <<
					"The number of specified mole fractions must equal the number of species."
					          <<
					std::endl;

					return;
				}

				// Normalize
				double norm = 0;
				for(int i = 0; i < speciesCount; i++)
					norm += moleFractions[i];

				for(int i = 0; i < speciesCount; i++)
					moleFractions[i] /= norm;

				// Initialize counts and specify species 1 as all.
				std::vector<double> counts(speciesCount, 0.0);
				int n = this->GetSiteCount();
				for(int i = 0; i < n; i++)
				{
					auto* site = this->SelectSite(i);
					counts[site->GetSpecies()-1]++;
				}

				// Loop through a few times to make sure we sample all points.
				for(int k = 0; k < 3; k++)
					for(int i = 0; i < n; i++)
					{
						auto* site = this->SelectRandomSite();
						int curr = site->GetSpecies();

						if(counts[curr-1]/n > moleFractions[curr-1])
							for(int j = 1; j <= speciesCount; j++)
								if(j != curr && counts[j-1]/n <
								   moleFractions[j-1])
								{
									counts[curr-1]--;
									site->SetSpecies(j);
									counts[j-1]++;
									break;
								}
					}
			}

			// Clone Model.
			virtual BaseModel* Clone() const = 0;

			// Destructor.
			virtual ~BaseModel(){}
	};
}
