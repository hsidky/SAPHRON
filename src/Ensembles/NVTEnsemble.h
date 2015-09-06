#pragma once

#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Worlds/WorldManager.h"
#include "../Rand.h"
#include "../Worlds/World.h"
#include "Ensemble.h"
#include <cmath>

namespace SAPHRON
{
	// Class for simple NVT ensemble - also known as "canonical" ensemble.This
	// calls moves off the move queue and uses the Metropolis criteria for acceptance
	// probability, A(i->j) = min(1,exp(DE/kB*T)).
	class NVTEnsemble : public Ensemble
	{
		private:

			// Pointer to world manager.
			WorldManager* _wmanager;

			// Pointer to force field manager.
			ForceFieldManager* _ffmanager;

			// Pointer to move manager.
			MoveManager* _mmanager;

			// Acceptance map.
			AcceptanceMap _accmap;

			inline void UpdateAcceptances()
			{
				for(auto& move : *_mmanager)
					_accmap[move->GetName()] = move->GetAcceptanceRatio();
			}

			void Iterate();

		protected:
			
			// Visit children.
			virtual void VisitChildren(Visitor& v) override
			{
				_mmanager->AcceptVisitor(v);
				_ffmanager->AcceptVisitor(v);
			}

		public:
			NVTEnsemble(WorldManager* wmanager,
			            ForceFieldManager* ffmanager,
			            MoveManager* mmanager) :
				_wmanager(wmanager), _ffmanager(ffmanager), 
				_mmanager(mmanager), _accmap()
			{
				// Evaluate energies of systems. 
				for(auto& world : *_wmanager)
				{
					auto EP = _ffmanager->EvaluateHamiltonian(*world);
					world->SetEnergy(EP.energy);
					world->SetPressure(EP.pressure);
				}

				UpdateAcceptances();
			}

			// Run the NVT ensemble for a specified number of iterations.
			virtual void Run(int iterations) override;

			// Get ratio of accepted moves.
			virtual AcceptanceMap GetAcceptanceRatio() const override
			{
				return _accmap;
			}

			virtual std::string GetName() const override { return "NVT"; }
	};
}
