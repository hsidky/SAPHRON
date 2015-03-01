#include "../Ensembles/NVTEnsemble.h"
#include "../Ensembles/SemiGrandDOSEnsemble.h"
#include "../Ensembles/WangLandauDOSEnsemble.h"
#include "../Histogram.h"
#include "ConsoleVisitor.h"
#include <algorithm>
#include <iterator>

using namespace Ensembles;
using namespace std;
using namespace Simulation;

namespace Visitors
{
	void ConsoleVisitor::VisitInternal(WangLandauDOSEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			cout << "Iteration: " << this->GetIteration() << " ";
		if (this->Flags.energy)
			cout << "Energy: " << e->GetEnergy() / e->GetModelSiteCount() << " ";

		// Get all DOS properties.
		VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));

		cout << endl;
	}

	void ConsoleVisitor::VisitInternal(SemiGrandDOSEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			cout << "Iteration: " << this->GetIteration() << " ";
		if (this->Flags.energy)
			cout << "Energy: " << e->GetEnergy() / e->GetModelSiteCount() << " ";
		if(this->Flags.temperature)
			cout << "Temperature: " << e->GetTemperature() << " ";
		if(this->Flags.composition)
		{
			auto comp = e->GetComposition();
			cout << "Composition : ";
			std::copy(comp.begin(), comp.end(), std::ostream_iterator<int>(std::cout, " "));
		}

		// Get all DOS properties.
		VisitInternal(static_cast<DensityOfStatesEnsemble<Site>*>(e));

		cout << endl;
	}

	void ConsoleVisitor::VisitInternal(DensityOfStatesEnsemble<Site>* e)
	{
		if(!this->Flags.dos)
			return;

		if (this->Flags.dos_flatness)
			cout << "Flatness: " << e->GetFlatness() << " ";
		if (this->Flags.dos_walker)
			cout << "Walker ID: " << e->GetWalkerID() << " ";
		if(this->Flags.dos_scale_factor)
			cout << "Scale factor: " << e->GetScaleFactor() << " ";
		if(this->Flags.dos_interval)
		{
			auto interval = e->GetParameterInterval();
			cout << "Interval: " << interval.first << " " << interval.second <<
			" ";
		}
		if (this->Flags.dos_values)
		{
			cout << "DOS: ";
			auto* dos = e->GetDensityOfStates();
			std::copy(dos->begin(), dos->end(),
			          std::ostream_iterator<double>(std::cout, " "));
		}
	}

	void ConsoleVisitor::VisitInternal(NVTEnsemble<Site>* e)
	{
		if (this->Flags.iterations)
			cout << "Iteration: " << this->GetIteration() << " ";
		if (this->Flags.energy)
			cout << "Energy: " << e->GetEnergy()/e->GetModelSiteCount() << " ";
		if (this->Flags.temperature)
			cout << "Temperature: " << e->GetTemperature() << " ";
		cout << endl;
	}

	void ConsoleVisitor::VisitInternal(Models::BaseModel*)
	{
	}

	void ConsoleVisitor::VisitInternal(Histogram* h)
	{
		if(!this->Flags.histogram)
			return;

		if(this->Flags.hist_values)
		{
			cout << "Histogram: ";
			auto* hist = h->GetHistogramPointer();
			std::copy(hist->begin(), hist->end(),
			          std::ostream_iterator<int>(std::cout, " "));
		}
		if(this->Flags.hist_bin_count)
			cout << "Bin count: " << h->GetBinCount() << " ";
		if(this->Flags.hist_lower_outliers)
			cout << "Lower outliers: " << h->GetLowerOutlierCount() << " ";
		if(this->Flags.hist_upper_outliers)
			cout << "Upper outliers: " << h->GetUpperOutlierCount() << " ";

		cout << endl;
	}

	void ConsoleVisitor::VisitInternal(Site* s)
	{
		if(!this->Flags.site)
			return;

		if (this->Flags.site_coordinates)
			cout << "Coordinates: "  << s->GetXCoordinate() << " " <<
			s->GetYCoordinate() << " " << s->GetZCoordinate() << " ";
		if (this->Flags.site_unit_vectors)
			cout << "Unit Vectors: " << s->GetXUnitVector() << " " <<
			s->GetYUnitVector() << " " << s->GetZUnitVector() << " ";
		if (this->Flags.site_species)
			cout << "Species: " << s->GetSpecies() << " ";
		if (this->Flags.site_neighbors)
		{
			auto& neighbors = s->GetNeighbors();
			cout << "Neighbors: ";
			std::copy(neighbors.begin(), neighbors.end(),
			          std::ostream_iterator<int>(std::cout, " "));
		}

		cout << endl;
	}
}
