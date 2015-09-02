#pragma once

#include "../Particles/Particle.h"
#include "../Worlds/World.h"

// Forward declare.
namespace Json {
	class Value;
}

namespace SAPHRON
{
	// Forward declare.
	class Move;

	// Typedefs.
	typedef std::vector<Move*> MoveList; 
	
	// Abstract base class for a Monte Carlo move.
	class Move
	{
	public:
		virtual ~Move(){}

		// Get acceptance ratio. 
		virtual double GetAcceptanceRatio() = 0;

		// Reset acceptance ratio.
		virtual void ResetAcceptanceRatio() = 0;

		// Perform the required draw for the move. This involved drawing particles if necessary.
		// If moves are performed on particles, pointers are pushed to the particles 
		// container reference for energy evaluation. If it's a world move that requires no particles,
		// simply resize the container to zero.
		virtual void Draw(World& world, ParticleList& particles) = 0;

		// Perform a move on draw. If the entire world requires an energy
		// re-evaluation (such as volume scaling) then function returns true. Otherwise, it 
		// returns false.
		virtual bool Perform(World& world, ParticleList& particles) = 0;

		// 
		virtual double AcceptanceProbability() = 0;

		// Undo a move.
		virtual void Undo() = 0;

		// Get move name. 
		virtual std::string GetName() = 0;

		// Get seed.
		virtual int GetSeed() { return 0; }

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
