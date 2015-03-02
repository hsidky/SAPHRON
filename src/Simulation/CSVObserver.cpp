#include "../Ensembles/NVTEnsemble.h"
#include "../Ensembles/SemiGrandDOSEnsemble.h"
#include "../Ensembles/WangLandauDOSEnsemble.h"
#include "../Histogram.h"
#include "CSVObserver.h"
#include <algorithm>
#include <iterator>

using namespace Ensembles;
using namespace std;
using namespace Simulation;

namespace Simulation
{
	void CSVObserver::VisitInternal(NVTEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			*_ensemblefs << this->GetIteration() << ",";
		if (this->Flags.energy)
			*_ensemblefs << e->GetEnergy() << ",";
		if (this->Flags.temperature)
			*_ensemblefs << e->GetTemperature() << ",";
		*_ensemblefs << endl;
	}

	void CSVObserver::VisitInternal(WangLandauDOSEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			*_ensemblefs << this->GetIteration() << ",";
		if (this->Flags.energy)
			*_ensemblefs << e->GetEnergy() << ",";

		// Get all DOS properties.
		VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));

		*_ensemblefs << endl;
	}

	void CSVObserver::VisitInternal(SemiGrandDOSEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			*_ensemblefs << this->GetIteration() << ",";
		if (this->Flags.energy)
			*_ensemblefs << e->GetEnergy() << ",";
		if(this->Flags.temperature)
			*_ensemblefs << e->GetTemperature() << ",";
		if(this->Flags.composition)
		{
			auto comp = e->GetComposition();
			std::copy(comp.begin(), comp.end(), std::ostream_iterator<int>(*_ensemblefs, ","));
		}

		// Get all DOS properties.
		VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));

		*_ensemblefs << endl;
	}

	void CSVObserver::VisitInternal(DensityOfStatesEnsemble<Site>* e)
	{
		if(!this->Flags.dos)
			return;

		if(this->Flags.iterations)
			*_dosfs << this->GetIteration() << ",";
		if (this->Flags.dos_walker)
			*_dosfs << e->GetWalkerID() << ",";
		if(this->Flags.dos_scale_factor)
			*_dosfs << e->GetScaleFactor() << ",";
		if (this->Flags.dos_flatness)
			*_dosfs << e->GetFlatness() << ",";
		if(this->Flags.dos_interval)
		{
			auto interval = e->GetParameterInterval();
			*_dosfs << interval.first << "," << interval.second << ",";
		}
		if (this->Flags.dos_values)
		{
			auto* dos = e->GetDensityOfStates();
			std::copy(dos->begin(), dos->end(), std::ostream_iterator<double>(*_dosfs, ","));
		}

		*_dosfs << endl;
	}

	void CSVObserver::VisitInternal(Models::BaseModel*)
	{
	}

	void CSVObserver::VisitInternal(Histogram* h)
	{
		if(!this->Flags.histogram)
			return;

		if(this->Flags.iterations)
			*_histfs << this->GetIteration() << ",";
		if(this->Flags.hist_bin_count)
			*_histfs << h->GetBinCount() << ",";
		if(this->Flags.hist_lower_outliers)
			*_histfs << h->GetLowerOutlierCount() << ",";
		if(this->Flags.hist_upper_outliers)
			*_histfs << h->GetUpperOutlierCount() << ",";

		if(this->Flags.hist_values)
		{
			auto* hist = h->GetHistogramPointer();
			std::copy(hist->begin(), hist->end(),
			          std::ostream_iterator<int>(*_histfs, ","));
		}

		*_histfs << endl;
	}

	void CSVObserver::VisitInternal(Site* s)
	{
		if(!this->Flags.site)
			return;

		if(this->Flags.iterations)
			*_sitefs << this->GetIteration() << ",";
		if (this->Flags.site_coordinates)
			*_sitefs << s->GetXCoordinate() << ","
			         << s->GetYCoordinate() << ","
			         << s->GetZCoordinate() << ",";
		if (this->Flags.site_unit_vectors)
			*_sitefs << s->GetXUnitVector() << ","
			         << s->GetYUnitVector() << ","
			         << s->GetZUnitVector() << ",";
		if (this->Flags.site_species)
			*_sitefs << s->GetSpecies() << ",";
		if (this->Flags.site_neighbors)
		{
			auto& neighbors = s->GetNeighbors();
			std::copy(neighbors.begin(), neighbors.end(), std::ostream_iterator<int>(*_sitefs, ","));
		}

		*_sitefs << endl;
	}
}
