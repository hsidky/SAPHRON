#pragma once

#include "Particle.h"

namespace SAPHRON
{
	// The most basic particle. This represents a site on a lattice.
	class Site : public Particle
	{
		private:
			Position _position;
			Director _director;

		public:

			// Initializes a site at a given position with a given director and identifier.
			Site(Position position, Director director, std::string identifier)
				: Particle(identifier), _position(position), _director(director){}

			// Gets site position.
			virtual Position GetPosition() const override
			{
				return _position;
			}

			// Sets site position.
			virtual void SetPosition(const Position& position) override
			{
				_position = position;
			}

			// Gets site director.
			virtual Director GetDirector() const override
			{
				return _director;
			}

			// Sets site director.
			virtual void SetDirector(const Director& director) override
			{
				_director = director;
			}

			// Evaluates the Hamiltonian of the site.
			virtual double EvaluateHamiltonian() override
			{
				return 1.0;
			}

			// Get descendants of the site (none).
			virtual std::vector<Particle*> GetDescendants()
			{
				return {this};
			}

			virtual Particle* Clone() const override
			{
				return new Site(static_cast<const Site&>(*this));
			}
	};
}
