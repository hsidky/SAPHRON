#pragma once 

#include <istream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "json/json.h"

namespace SAPHRON
{
	class SimBuilder 
	{
		private: 
			
			struct WorldProps
			{

			};

			Json::Reader _reader;
			Json::Value _root;

			bool _errors;

			// Error messages.
			std::vector<std::string> _emsgs;

			bool ValidateWorld(Json::Value world)
			{
				bool err = false;
				// World tag exists.
				if(!world)
				{
					_emsgs.push_back("No world has been specified.");
					return false;
				}

				// World type specified.
				std::string type = world.get("type", "").asString();
				if(type.length() == 0)
				{
					_emsgs.push_back("No world type has been specified.");
					err = true;
				}

				// Validate that the world type is a valid entry.
				std::vector<std::string> types =  {"Simple"};
				auto it = std::find(types.begin(), types.end(), type);
				if(it == types.end())
				{
					_emsgs.push_back("The type of world specified is invalid.");

					std::ostringstream s;
					std::copy(types.begin(), types.end(), 
							  std::ostream_iterator<std::string>(s," "));
					_emsgs.push_back("Valid entries are: " + s.str());

					err = true;
				}

				// Simple world type.
				if(type == types[0])
				{
					auto wsize = world["size"];
					if(!wsize.isArray() || wsize.size() != 3)
					{
						_emsgs.push_back("The world size must be a 1x3 array.");
						err = true;
					}
					if(wsize[0].asDouble() <= 0 || 
					   wsize[1].asDouble() <= 0 || 
					   wsize[2].asDouble() <= 0)
					{
						_emsgs.push_back("All world size elements must be greater than zero.");
						err = true;	
					}
				}

				double rcutoff = world["cutoff_radius"].asDouble();

				return !err;
			}

		public:
			SimBuilder() : _reader(), _root(), _errors(false), _emsgs(0) {}

			// Parse stream input. Returns true if parse was successful or 
			// false if otherwise. Error(s) can be obtained via GetParsingErrors().
			bool ParseInput(std::istream& is)
			{
				if(!_reader.parse(is, _root))
				{
					_errors = true;
					_emsgs.push_back(_reader.getFormattedErrorMessages());
					return false;
				}

				// Validate world. 
				if(!ValidateWorld(_root["world"]))
				{
					_errors = true;
					return false;
				}

				return true;
			} 

			std::vector<std::string> GetErrorMessages()
			{
				return _emsgs;
			}



	};
}