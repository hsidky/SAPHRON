#pragma once 

#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include "Requirement.h"
#include "StringRequirement.h"
#include "IntegerRequirement.h"
#include "NumberRequirement.h"
#include "DependencyRequirement.h"

namespace Json
{
	class ObjectRequirement : public Requirement
	{
	private:
		std::map<std::string, Requirement*> _children;
		DependencyRequirement* _dependency;

		std::list<std::string> _required;
		bool _moreProps, _setMin, _setMax;
		unsigned int _min, _max;

	public:
		ObjectRequirement() : 
		_children(), _dependency(nullptr), _required(), _moreProps(true), 
		_setMin(false), _setMax(false), _min(0), _max(0)
		{}

		~ObjectRequirement()
		{
			for(auto& c : _children)
				delete c.second;
			_children.clear();

			delete _dependency;
		}

		virtual void ClearErrors() override
		{
			for(auto& c : _children)
				c.second->ClearErrors();

			if(_dependency != nullptr)
				_dependency->ClearErrors();

			Requirement::ClearErrors();
		}

		virtual void ClearNotices() override
		{
			for(auto& c : _children)
				c.second->ClearNotices();

			if(_dependency != nullptr)
				_dependency->ClearNotices();

			Requirement::ClearNotices();
		} 

		virtual void Reset() override
		{
			for(auto& c : _children)
				delete c.second;
			_children.clear();

			_moreProps = true;
			_setMin = _setMax = false;
			_min = _max = 0;
			_required.clear();
			delete _dependency;
			_dependency = nullptr;
			ClearErrors();
			ClearNotices();
		}

		virtual void Parse(Value json, std::string path) override
		{
			Reset();

			// Additional properties.
			if(json.isMember("additionalProperties"))
			{
				if(json["additionalProperties"].isBool())
					_moreProps = json["additionalProperties"].asBool();
				else if(json["additionalProperties"].isObject())
					json["properties"]["additionalProperties"] = json["additionalProperties"];
			}

			// properties.
			if(json.isMember("properties") && json["properties"].isObject())
			{
				auto& props = json["properties"];
				auto names = props.getMemberNames();
				int i = 0;
				for(auto& prop : props)
				{
					if(prop.isObject())
					{
						// Load up appropriate requirement type.
						if(prop["type"].asString() == "string")
							_children.insert({names[i], new StringRequirement()});
						else if(prop["type"].asString() == "integer")
							_children.insert({names[i], new IntegerRequirement()});
						else if(prop["type"].asString() == "number")
							_children.insert({names[i], new NumberRequirement()});
						else if(prop["type"].asString() == "object")
							_children.insert({names[i], new ObjectRequirement()});
					}

					_children[names[i]]->Parse(prop, path + "/" + names[i]);
					++i;
				}
			}

			// Required properties.
			if(json.isMember("required") && json["required"].isArray())
			{
				for(auto& requirement : json["required"])
					_required.push_back(requirement.asString());
			}

			// Min property count.
			if(json.isMember("minProperties") && json["minProperties"].isUInt())
			{
				_setMin = true;
				_min = json["minProperties"].asInt();
			}

			// Max property count.
			if(json.isMember("maxProperties") && json["maxProperties"].isUInt())
			{
				_setMax = true;
				_max = json["maxProperties"].asInt();
			}

			// Dependencies
			if(json.isMember("dependencies") && json["dependencies"].isObject())
			{
				_dependency = new DependencyRequirement();
				_dependency->Parse(json["dependencies"], path);
			}
		}

		virtual void Validate(const Value& json, std::string path) override
		{
			if(!json.isObject())
			{
				PushError(path + ": Must be of type \"object\"");
				return;
			}

			if(_setMin && json.size() < _min)
				PushError(path + ": Object must contain at least " + std::to_string(_min) + " properties");

			if(_setMax && json.size() > _max)
				PushError(path + ": Object must contain at most " + std::to_string(_max) + " properties");


			// Check dependencies. 
			if(_dependency != nullptr)
			{
				_dependency->Validate(json, path);
				if(_dependency->HasErrors())
					for(const auto& error : _dependency->GetErrors())
						PushError(error);
				if(_dependency->HasNotices())
					for(const auto& notice : _dependency->GetNotices())
						PushNotice(notice);
			}

			// Copy so we can pop items off the list.
			auto rprops = _required;

			auto names = json.getMemberNames();
			int i = 0;
			for(auto& prop : json)
			{
				Requirement* requirement = nullptr; 
				auto it = _children.find(names[i]);
				if(it != _children.end())
					requirement = it->second;					
				else if(_children.find("additionalProperties") != _children.end())
					requirement = _children["additionalProperties"];
				else if(!_moreProps)
					PushError(path + ": Invalid property \"" + names[i] + "\" specified");

				if(requirement != nullptr)
				{
					requirement->Validate(prop, path + "/" + names[i]);
					if(requirement->HasErrors())
						for(const auto& error : requirement->GetErrors())
							PushError(error);
					if(requirement->HasNotices())
						for(const auto& notice : requirement->GetNotices())
							PushNotice(notice);
				}

				rprops.remove(names[i]);
				++i;
			}

			if(_required.size() && rprops.size() != 0)
			{
				std::string msg = std::accumulate(rprops.begin(), rprops.end(), std::string(), 
    				[](const std::string& a, const std::string& b) -> std::string { 
        				return a + (a.length() > 0 ? ", " : "") + b; 
    			});
				PushError(path + ": Missing properties: " + msg);
			}
		}
	};
}