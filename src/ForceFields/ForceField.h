#pragma once

#include "../Particles/Particle.h"
#include "../Properties/Energy.h"
#include "../Properties/Pressure.h"
#include "../Properties/EPTuple.h"
#include "../JSON/Serializable.h"

namespace SAPHRON
{
	// Forward delcare.
	class ForceFieldManager;
	class ForceField;

	// Typedefs. 
	typedef std::vector<ForceField*> FFList;

	// Abstract base class for a force field. Represents the scalar interaction potential between
	// two bodies (particles). It calculates energy and intermolecular virial contribution. 
	// The virial contribution is written as in Allan & Tildesley Eq. 2.60 and 2.63.
	// The forcefield implementation should return w(r)/r^2. This corresponds to 1/r*dv(r)/dr.
	class ForceField
	{
	public:
		// Returns the potential and virial contribution between two particle. The distance vector 
		// between the two particles is provided by the FFManager with minimum image applied. The 
		// cutoff radius for the corresopnding system is also supplied.
		virtual Interaction Evaluate(const Particle& p1, 
									 const Particle& p2, 
									 const Position& rij, 
									 double rcut) = 0;

		// Evaluates the energy tail correction term. This is precisely integral(u(r)*r^2,rc,inf). The 
		// remainder is taken care of by the forcefield manager.
		virtual double EnergyTailCorrection(double) { return 0.0; }

		// Evalutes the pressure tail correction term.
		virtual double PressureTailCorrection(double) { return 0.0; }

		// Serialize 
		virtual void Serialize(Json::Value& root) const = 0;

		// Build a non-bonded forcefield from a JSON node. Returns a pointer to the built 
		// FF in addition to adding it to the FFM. If return value is nullptr, then an 
		// unknown error occurred. Will throw BuildException on failure. Object lifetime 
		// is the caller's responsibility.
		static ForceField* BuildNonBonded(const Json::Value& json, ForceFieldManager* ffm);

		// Overloaded function allowing JSON path specification.
		static ForceField* BuildNonBonded(const Json::Value& json, 
										  ForceFieldManager* ffm, 
										  const std::string& path);

		// Build an electrostatic forcefield from a JSON node. Returns a pointer to the built 
		// FF in addition to adding it to the FFM. If return value is nullptr, then an 
		// unknown error occurred. Will throw BuildException on failure. Object lifetime 
		// is the caller's responsibility.
		static ForceField* BuildElectrostatic(const Json::Value& json, ForceFieldManager* ffm);

		// Overloaded function allowing JSON path specification.
		static ForceField* BuildElectrostatic(const Json::Value& json, 
										 	  ForceFieldManager* ffm, 
										  	  const std::string& path);

		// Builds forcefields from base tree root["forcefields"] and adds them to the Forcefield manager.
		// It also adds all initialized pointers to the fflist array passed in. Throws exception on 
		// failure. Object lifetime management is caller's responsibility.
		static void BuildForceFields(const Json::Value& json, ForceFieldManager* ffm, FFList& fflist);

		virtual ~ForceField() {}
	};
}
