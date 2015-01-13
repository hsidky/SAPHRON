#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "MockLogger.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Loggers;

// Expected magnetization for the NVT Ensemble on the 3D Ising model at various temperatures.
TEST(NVTEnsemble, IsingModelMagnetization)
{
	int n = 37;
	Ising3DModel model(n, 1);
	FlipSpinMove move;
	MockLogger l;
	NVTEnsemble<Site> s(model, 1.0);

	// Lambda function for logger
	double fm = 0;
	auto magnetization = [&fm] (BaseModel & model) {
		double m = 0.0;
		auto c = model.GetSiteCount();
		for(int i = 0; i < c; i++)
			m += model.SelectSite(i)->GetZUnitVector();
		m /= c;
		fm = m;
		return m;
	};

	// Add magnetization to logger.
	l.AddLoggableProperty(magnetization);
	s.AddLogger(l);
	s.AddMove(move);

	// Iteratte
	for(int i = 0; i < 100; i++)
		s.Sweep();

	ASSERT_NEAR(1.0, fm, 0.1);

	// Change temperature
	s.SetTemperature(5.0);

	// Iteratte
	for(int i = 0; i < 100; i++)
		s.Sweep();

	ASSERT_NEAR(0.0, fm, 0.1);
}
