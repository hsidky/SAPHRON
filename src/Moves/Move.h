#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"

// Forward declare.
namespace Json {
	class Value;
}

namespace SAPHRON
{
	// Move override flag.
	enum MoveOverride
	{
		None,
		ForceAccept,
		ForceReject
	};

	// Forward declare.
	class Move;
	class ForceFieldManager;
	class WorldManager;
	class DOSOrderParameter;

	// Typedefs.
	typedef std::vector<Move*> MoveList; 
	
	// Abstract base class for a Monte Carlo move.
	class Move
	{
	public:
		virtual ~Move(){}

		// Get acceptance ratio. 
		virtual double GetAcceptanceRatio() const = 0;

		// Reset acceptance ratio.
		virtual void ResetAcceptanceRatio() = 0;

		// Perform a move given the world manager and forcefield manager.
		// An implemented move should respect constness and not change the state 
		// of the instance. This is important to ensure thread safety!
		virtual void Perform(WorldManager* wm, ForceFieldManager* ffm, const MoveOverride& override) = 0;

		// Perform move interface for DOS ensemble.
		virtual void Perform(World* world, ForceFieldManager* ffm, DOSOrderParameter* op, const MoveOverride& override) = 0;

		// Get move name. 
		virtual std::string GetName() const = 0;

		// Get seed.
		virtual int GetSeed() const { return 0; }

		// Clone a move.
		virtual Move* Clone() const = 0;

		// Builds a move from a JSON node. Returns a pointer to the built Move in addition to adding it 
		// to the move manager. If return value is nullptr, then an unknown error occurred. It will throw 
		// a BuildException on failure. Object lifetime is the caller's responsibility. 
		static Move* BuildMove(const Json::Value& json, MoveManager* mm);

		// Overloaded function allowing JSON path specification.
		static Move* BuildMove(const Json::Value& json, MoveManager* mm, const std::string& path);

		// Builds moves from a base tree root["moves"] and adds them to the Move manager. 
		// It also adds all initialized pointers to the mvlist array passed in. Throws exception
		// on failure. Object lifetime management is caller's responsibility. 
		static void BuildMoves(const Json::Value& json, MoveManager* mm, MoveList& mvlist);
	};
}
