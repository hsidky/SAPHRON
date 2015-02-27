#include "../Ensembles/NVTEnsemble.h"
#include "ConsoleVisitor.h"

using namespace Ensembles;
using namespace std;
using namespace Simulation;

namespace Visitors
{
	void ConsoleVisitor::Update(SimEvent& e)
	{
		this->SetIteration(e.GetObservable()->GetSimIteration());
	}

	void ConsoleVisitor::Visit(NVTEnsemble<Site>* e)
	{
		if(this->IsObservableIteration())
		cout << "Temperature: " << e->GetTemperature() << ", Iterations: " <<
		e->GetSweepCount() << endl;
	}

	void ConsoleVisitor::Visit(Models::BaseModel*)
	{
	}

	void ConsoleVisitor::Visit(Site* s)
	{
		//std::cout << "Coordinates: "  << s->GetXCoordinate() << " " <<
		//s->GetYCoordinate() << " " << s->GetZCoordinate() << endl;
	}
}
