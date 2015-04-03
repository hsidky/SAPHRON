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

			// Initializes a site at a given position with a given director and species.
			Site(Position position, Director director, std::string species)
				: Particle(species), _position(position), _director(director){}

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

			// Sets site position.
			virtual void SetPosition(Position && position) override
			{
				_position = position;
			}

			// Gets site director.
			virtual Director GetDirector() const override
			{
				return _director;
			}

			virtual const Director& GetDirectorRef() const override
			{
				return _director;
			}

			// Sets site director.
			virtual void SetDirector(const Director& director) override
			{
				_director[0] = director[0];
				_director[1] = director[1];
				_director[2] = director[2];
			}

			// Sets site director.
			virtual void SetDirector(Director && director) override
			{
				_director = director;
			}

			virtual void SetDirector(double ux, double uy, double uz) override
			{
				_director[0] = ux;
				_director[1] = uy;
				_director[2] = uz;
 			}

			// Get descendants of the site (none).
			virtual std::vector<Particle*> GetChildren() override
			{
				return {};
			}

			virtual Particle* Clone() const override
			{
				return new Site(static_cast<const Site&>(*this));
			}
	};
}
