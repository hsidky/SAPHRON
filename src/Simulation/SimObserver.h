#pragma once

#include "../Visitors/Visitor.h"
#include "SimEvent.h"

using namespace Visitors;

namespace Simulation
{
	// Abstract class for objects wanting to observe a simulation.
	class SimObserver : public Visitor
	{
		private:
			unsigned int _frequency = 1;
			int _iteration = 0;
			bool _forceObserve = false;

		protected:

			const SimFlags Flags;

			// Get iteration count.
			int GetIteration()
			{
				return _iteration;
			}

			// Set iteration count.
			int SetIteration(int i)
			{
				return _iteration = i;
			}

			// Get logging frequency.
			unsigned int GetFrequency()
			{
				return _frequency;
			}

			// Set logging frequency.
			unsigned int SetFrequency(int f)
			{
				return _frequency = f;
			}

			bool IsObservableIteration()
			{
				return _iteration % _frequency == 0 || _forceObserve;
			}

			virtual void VisitInternal(Ensembles::WangLandauDOSEnsemble<Site>* e) = 0;
			virtual void VisitInternal(Ensembles::DensityOfStatesEnsemble<Site>* e) = 0;
			virtual void VisitInternal(Ensembles::NVTEnsemble<Site>* e) = 0;
			virtual void VisitInternal(Models::BaseModel* m) = 0;
			virtual void VisitInternal(Site* s) = 0;
			virtual void VisitInternal(Histogram* h) = 0;

		public:

			// Initialize a SimObserver class with a specified observation frequency.
			SimObserver(SimFlags flags, unsigned int frequency = 1)
				: _frequency(frequency), Flags(flags){}

			// Update observer when simulation has changed.
			void Update(SimEvent& e);

			void Visit(Ensembles::WangLandauDOSEnsemble<Site>* e) override
			{
				if (IsObservableIteration())
					VisitInternal(e);
			}

			void Visit(Ensembles::DensityOfStatesEnsemble<Site>* e) override
			{
				if (IsObservableIteration())
					VisitInternal(e);
			}

			void Visit(Ensembles::NVTEnsemble<Site>* e) override
			{
				if (IsObservableIteration())
					VisitInternal(e);
			};

			void Visit(Models::BaseModel* m) override
			{
				if (IsObservableIteration())
					VisitInternal(m);
			};

			void Visit(Site* s) override
			{
				if (IsObservableIteration())
					VisitInternal(s);
			};

			void Visit(Histogram* h) override
			{
				if(IsObservableIteration())
					VisitInternal(h);
			}
	};
}
