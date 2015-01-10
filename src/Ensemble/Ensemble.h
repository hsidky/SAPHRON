#pragma once

#include "../Models/BaseModel.h"

using namespace Models;

namespace Ensemble
{
    // Base class for simulation Ensembles.
    class Ensemble
    {
        private:
            // Pointer to model.
            BaseModel* model;

        public:
            Ensemble (BaseModel* model): model(model) {};
            virtual ~Ensemble ();
    };
}
