#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/Models/Ising3DModel.h"
#include "../src/Moves/FlipSpinMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"

using namespace Ensembles;
using namespace Models;

TEST(NVTEnsemble, DefaultConstructor)
{
    int n = 37;
    Ising3DModel model(n, 1);
    FlipSpinMove move;
    NVTEnsemble<Site> s(model);

    // TODO: Complete testing Ensemble
}
