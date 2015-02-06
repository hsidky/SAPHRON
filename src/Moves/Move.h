#pragma once

#include "../Site.h"

namespace Moves
{
	// Abstract base class for a Monte Carlo move.
	template<typename T>
	class Move
	{
		private:
			bool _forceAccept = false;

		public:
			virtual ~Move(){}

			// Perform a Monte Carlo move.
			virtual void Perform(T& type) = 0;

			// Undo a Monte Carlo move.
			virtual void Undo() = 0;

			// Specify if a move is to be accepted unconditionally.
			// It is the responsibility of the derived classes to enforce
			// this on Undo().
			bool SetForceAccept(bool force)
			{
				return _forceAccept = force;
			}

			// Detemrine if a move is to be accepted unconditionally.
			bool ForceAccept()
			{
				return _forceAccept;
			}
	};
}
