#pragma once 

#include "Requirement.h"
#include "StringRequirement.h"
#include "IntegerRequirement.h"
#include "NumberRequirement.h"
#include "ObjectRequirement.h"

namespace Json
{
	class ArrayRequirement : public Requirement
	{
	private:
		std::vector<Requirement*> _items;
		Requirement* _item;

		bool _tuple;

	public:
		ArrayRequirement() : 
		_items(), _item(nullptr) {}
		~ArrayRequirement() 
		{
			for(auto& c : _items)
				delete c;
			_items.clear();

			delete _item;
		}

		virtual void ClearErrors() override
		{
			if(_item != nullptr)
				_item->ClearErrors();

			for(auto& c : _items)
				c->ClearErrors();

			Requirement::ClearErrors();
		}

		virtual void ClearNotices() override
		{
			if(_item != nullptr)
				_item->ClearNotices();

			for(auto& c : _items)
				c->ClearNotices();

			Requirement::ClearNotices();
		}

		virtual void Reset() override
		{
			for(auto& c : _items)
				delete c;
			_items.clear();

			delete _item;
		}

		virtual void Parse(Value json, std::string path) override
		{
			Reset();

			if(json.isMember("items") && json.isObject())
			{
				auto& type = json["items"]["type"];		

				// Load up apitemriate requirement type.
				if(type.asString() == "string")
					_item = new StringRequirement();
				else if(type.asString() == "integer")
					_item = new IntegerRequirement();
				else if(type.asString() == "number")
					_item = new NumberRequirement();
				else if(type.asString() == "object")
					_item = new ObjectRequirement();
				
				_item->Parse(json["items"], path);
			}
		}

		virtual void Validate(const Value& json, std::string path) override
		{
			if(!json.isArray())
			{
				PushError(path + ": Must be of type \"array\"");
				return;
			}

			if(_item != nullptr)
			{
				for(const auto& item : json)
					_item->Validate(item, path);

				// Merge errors.
				for(const auto& error : _item->GetErrors())
					PushError(error);

				for(const auto& notice : _item->GetNotices())
					PushNotice(notice);
			}
		}
	};
}