#pragma once 

#include <regex>
#include "Requirement.h"

namespace Json
{
	class StringRequirement : public Requirement 
	{
	private: 
		bool _minSet, _maxSet, _rgxSet;
		int _minLength, _maxLength;
		std::regex _regex;
		std::string _expr;
		std::string _path;

	public:
		StringRequirement() : 
		_minSet(false), _maxSet(false), _rgxSet(false),
		_minLength(0), _maxLength(0), _regex(), _expr(), _path()
		{}
		
		virtual void Reset() override
		{
			_minSet = false;
			_maxSet = false;
			_rgxSet = false;
			_minLength = 0;
			_maxLength = 0;
			_regex = "";
			_expr = "";
			_path  = "";
			ClearErrors();
			ClearNotices();
		}

		virtual void Parse(Value json, std::string path) override
		{
			_path = path;
			if(json.isMember("minLength") && json["minLength"].isInt())
			{
				_minSet = true;
				_minLength = json["minLength"].asInt();
			}
			
			if(json.isMember("maxLength") && json["maxLength"].isInt())
			{
				_maxSet = true;
				_maxLength = json["maxLength"].asInt();
			
			}

			if(json.isMember("pattern") && json["pattern"].isString())
			{
				_rgxSet = true;
				_expr = json["pattern"].asString();
				_regex = std::regex(_expr, std::regex::ECMAScript);
			}
		}

		virtual void Validate(Value json, std::string path) override
		{
			if(!json.isString())
				PushError(path + ": Must be of type \"string\".");
			
			if(_minSet && json.asString().length() < _minLength)
				PushError(path + ": Length must be greater than " + std::to_string(_minLength));
			
			if(_maxSet && json.asString().length() > _maxLength)
				PushError(path + ": Length must be less than " + std::to_string(_minLength));

			if(_rgxSet && !std::regex_match(json.asString(), _regex))
				PushError(path + ": String must match regular expression \"" + _expr + "\"");
		}
	};
}