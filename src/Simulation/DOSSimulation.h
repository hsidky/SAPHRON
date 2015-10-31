#pragma once 

#include "Simulation.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../Worlds/WorldManager.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Utils/Histogram.h"

namespace SAPHRON
{
	typedef std::pair<double, double> Interval;

	// Generalized Density-of-States (DOS) sampling. Based on original WL algorithm.
	// [1] Wang, F., & Landau, D. P. (2001). Physical Review Letters, 86, 2050–2053.
	// [2] Wang, F., & Landau, D. P. (2001). Physical Review E, 64, 1–16.
	class DOSSimulation : public Simulation
	{
		private: 
			// Pointer to world manager.
			WorldManager* _wmanager;

			// Pointer to force field manager.
			ForceFieldManager* _ffmanager;

			// Pointer to move manager.
			MoveManager* _mmanager;

			// Order parameter
			DOSOrderParameter* _orderp;

			// Histogram
			Histogram* _hist;

			// Acceptance map.
			AcceptanceMap _accmap;

			// Histogram reset frequency. 
			int _hreset; 

			// Convergence factor 
			double _f; 

			// Flatness.
			double _flatness;

			// Target flatness.
			double _targetFlatness;
		
			void Iterate();

			inline void UpdateAcceptances()
			{
				for(auto& move : *_mmanager)
					_accmap[move->GetName()] = move->GetAcceptanceRatio();
			}

		protected:

			// Visit children.
			virtual void VisitChildren(Visitor& v) const override
			{
				_hist->AcceptVisitor(v);
				_mmanager->AcceptVisitor(v);
				_ffmanager->AcceptVisitor(v);
				_wmanager->AcceptVisitor(v);
			}

		public:
			DOSSimulation(WorldManager* wm, 
						  ForceFieldManager* ffm, 
						  MoveManager* mm, 
						  DOSOrderParameter* dop,
						  Histogram* hist) : 
				_wmanager(wm), _ffmanager(ffm), _mmanager(mm), _orderp(dop), _hist(hist),
				_accmap(), _hreset(0), _f(1.0), _flatness(0.0), _targetFlatness(0.80)			
			{
				// Moves per iteration.
				int mpi = 0;

				// Evaluate energies of systems. 
				for(auto& world : *_wmanager)
				{
					auto EP = _ffmanager->EvaluateHamiltonian(*world);
					world->SetEnergy(EP.energy);
					world->SetPressure(EP.pressure);
					mpi += world->GetParticleCount();
				}

				this->SetMovesPerIteration(mpi);
				UpdateAcceptances();
			}

			virtual void Run(int iterations) override;


            // Reduces the convergence factor order by a specified multiple.
         	void ReduceConvergenceFactor(double order = 0.5)
         	{
            	// We store log of scale factor. So we simply multiply.
                _f = _f*order;
         	}

			/* Getters and setters */

			// Get ratio of accepted moves.
			virtual AcceptanceMap GetAcceptanceRatio() const override
			{
				return _accmap;
			}

			// Get current flatness.
			double GetFlatness() const { return _flatness; }

			// Get target flatness.
			double GetTargetFlatness() const { return _targetFlatness; }

			// Set target flatness.
			void SetTargetFlatness(double f) { _targetFlatness = f; }
			
			// Gets the log of convergence factor.
         	double GetConvergenceFactor() const { return _f; }
         	
         	// Sets the log of scale factor.
         	void SetConvergenceFactor(double f) { _f = f; }
        
        	// Gets the histogram reset frequency. 
        	double GetHistogramResetFrequency() const { return _hreset; }

        	// Set histogram reset frequency. 
        	void SetHistogramResetFrequency(int hreset) { _hreset = hreset; }

        	// Get ensemble name.
			virtual std::string GetName() const override { return "DOS"; }

			virtual void Serialize(Json::Value& json) const override
			{
				// Call parent first.
				Simulation::Serialize(json);

				// Serialize DOS Order parameter.
				_orderp->Serialize(json["orderparameter"]);
			}

         	// Accept a visitor.
			virtual void AcceptVisitor(Visitor& v) const override
			{
				v.Visit(*this);
				VisitChildren(v);
			}

			~DOSSimulation() {};
	};
}