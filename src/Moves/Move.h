#pragma once

#include "../Site.h"

namespace Moves
{
    // Abstract base class for a Monte Carlo move.
    template<typename T>
    class Move
    {
        public:
            virtual ~Move(){}

            // Perform a Monte Carlo move.
            virtual void Perform(T* type) = 0;

            // Undo a Monte Carlo move.
            virtual void Undo() = 0;
    };
}
