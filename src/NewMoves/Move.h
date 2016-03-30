#pragma once 

#include "../Utils/Types.h"

namespace SAPHRON 
{
	enum MoveOverride
	{
		None,
		Accept,
		Reject
	};

	// Forward declare.
	class WorldManager;
	class ForceFieldManager;

	// Abstract base class for a Monte Carlo move.
	class Move
	{
	private:
		int attempted_, rejected_;

	protected:
		void IncrementAttempts() { ++attempted_; }
		void IncrementRejections() { ++rejected_; }

	public:
		Move() : attempted_(0), rejected_(0) {}

		// Perform a move given the world manager and forcefield manager. 
		// An implemented move should respect constness and not change the state 
		// of the instance. This is important to ensure thread safety!
		virtual void Perform(WorldManager*,ForceFieldManager*, const MoveOverride&) = 0;

		// Get acceptacnce ratio.
		double GetAcceptanceRatio() const
		{
			return 1. - static_cast<double>(rejected_)/attempted_;
		}

		// Reset acceptance ratio.
		void ResetAcceptanceRatio()
		{
			attempted_ = 0;
			rejected_ = 0;
		}
	};
}