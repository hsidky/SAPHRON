#pragma once

#include "../Simulation/SimObserver.h"

namespace Simulation
{
	class ConsoleObserver : public SimObserver
	{
		public:
			ConsoleObserver(SimFlags flags, unsigned int frequency = 1)
				: SimObserver(flags, frequency){}

			virtual void VisitInternal(Ensembles::DensityOfStatesEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::WangLandauDOSEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::SemiGrandDOSEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::NVTEnsemble<Site>* e) override;
			virtual void VisitInternal(Models::BaseModel* m) override;
			virtual void VisitInternal(Site* s) override;
			virtual void VisitInternal(Histogram* h) override;
	};
}
