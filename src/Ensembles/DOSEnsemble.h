#pragma once 

#include "Ensemble.h"
#include "../DensityOfStates/DOSOrderParameter.h"
#include "../Worlds/World.h"
#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Rand.h"
#include "../Histogram.h"

namespace SAPHRON
{
	typedef std::pair<double, double> Interval;

	// Generalized Density-of-States (DOS) sampling. Based on original WL algorithm.
	// [1] Wang, F., & Landau, D. P. (2001). Physical Review Letters, 86, 2050–2053.
	// [2] Wang, F., & Landau, D. P. (2001). Physical Review E, 64, 1–16.
	class DOSEnsemble : public Ensemble
	{
		private: 

			// Total system energy.
			double _energy;

			// Energy interval.
			Interval _interval;

			// Accepted moves count.
			int _acceptedCount;

			// Histogram
			Histogram _hist;

			// Scale factor 
			double _sf; 

			// Flatness.
			double _flatness;

			// Reference to world.
			World& _world;
			
			// Reference to force field manager.
			ForceFieldManager& _ffmanager;

			// Reference to move manager.
			MoveManager& _mmanager;

			// Order parameter
			DOSOrderParameter& _orderp;

			// Random number generator.
			Rand _rand; 

			// List of drawn particles.
			ParticleList _particles;

			// Target flatness.
			double _targetFlatness;

			double AcceptanceProbability(double prevE, double prevO, double newE, double newO);
		
			void Iterate();

		protected:

			// Visit children.
			virtual void VisitChildren(Visitor& v) override
			{
				_world.AcceptVisitor(v);
			}

		public:
			DOSEnsemble(DOSOrderParameter& orderp, World& world, 
						ForceFieldManager& ffmanager, 
					    MoveManager& mmanager, Interval interval, int binCount, int seed = 1) : 
				_energy(0), _interval(interval), _acceptedCount(0), _hist(interval.first, interval.second, binCount), 
				_sf(1.0), _flatness(0), _world(world), _ffmanager(ffmanager), _mmanager(mmanager), 
				_orderp(orderp), _rand(seed), _particles(0), _targetFlatness(0.80)
			{
				_particles.reserve(10);
				_energy = ffmanager.EvaluateHamiltonian(world);
			}

			DOSEnsemble(DOSOrderParameter& orderp, World& world, 
						ForceFieldManager& ffmanager, 
					    MoveManager& mmanager, Interval interval, double binWidth, int seed = 1) : 
				_energy(0), _interval(interval), _acceptedCount(0), _hist(interval.first, interval.second, binWidth), 
				_sf(1.0), _flatness(0), _world(world), _ffmanager(ffmanager), _mmanager(mmanager), 
				_orderp(orderp), _rand(seed), _particles(0), _targetFlatness(0.80)
			{
				_particles.reserve(10);
				_energy = ffmanager.EvaluateHamiltonian(world);
			}

			virtual void Run(int iterations) override;

			virtual double GetEnergy() override
			{
				return _energy;
			}

			virtual double GetTemperature() override 
			{
				return _orderp.GetTemperature();
			}

			// Get ratio of accepted moves.
			virtual double GetAcceptanceRatio() override
			{
				return (double)_acceptedCount/(double)_world.GetParticleCount();
			}

			virtual double GetFlatness()
			{
				return _flatness;
			}

			// Get target flatness.
			double GetTargetFlatness()
			{
				return _targetFlatness;
			}

			// Set target flatness.
			void SetTargetFlatness(double f)
			{
				_targetFlatness = f;
			}

			// Get bin count.
			double GetBinCount()
			{
				return _hist.GetBinCount();
			}

			// Get interval.
			Interval GetInterval()
			{
				return _interval;
			}

			std::vector<double>* GetDensityOfStates()
			{
				return _hist.GetValuesPointer();
			}

			// Reset histogram.
			void ResetHistogram()
			{
				_hist.ResetHistogram();
			}

            // Reduces the scaling factor order by a specified multiple.
         	void ReduceScaleFactor(double order = 0.5)
         	{
            	// We store log of scale factor. So we simply multiply.
                _sf = _sf*order;
         	}

         	// Sets the scale factor.
         	void SetScaleFactor(double sf)
         	{
         		_sf = sf;
         	}

         	// Gets the scale factor.
         	virtual double GetScaleFactor()
         	{
         		return _sf;
         	}

         	// Accept a visitor.
			virtual void AcceptVisitor(class Visitor &v) override
			{
				v.Visit(this);
				VisitChildren(v);
			}

			~DOSEnsemble() {};
	};
}