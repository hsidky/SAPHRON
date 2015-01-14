#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Loggers/CSVLogger.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;
using namespace Loggers;

// Expected magnetization for the NVT Ensemble on the 3D Ising model at various temperatures.
TEST(CSVLogger, CSVFormatting)
{
	int n = 10;
	Ising3DModel model(n, 1);
	FlipSpinMove move;
	CSVLogger l("model.csv", "site.csv");
	NVTEnsemble<Site> s(model, 4.5);

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
	l.AddModelProperty("Magnetization", magnetization);
	l.AddSiteProperty("Spin", [] (Site & site){ return site.GetZUnitVector();
			  });
	l.AddSiteProperty("z-coordinate", [] (Site & site){ return site.GetZCoordinate();
			  });
	l.AddSiteProperty("y-coordinate", [] (Site & site){ return site.GetYCoordinate();
			  });
	l.AddSiteProperty("x-coordinate", [] (Site & site){ return site.GetXCoordinate();
			  });

	s.AddLogger(l);
	s.AddMove(move);
	l.WriteHeaders();

	// Iterate
	for(int i = 0; i < 100; i++)
		s.Sweep();
}
