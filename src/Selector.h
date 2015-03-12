#pragma once

#include "Site.h"
#include <vector>

// Class that allows selection of elements from a vector based on criteria
// specified by the user.
template<class T>
class Selector
{
	private:
		std::vector<T>* _container;

	public:
		Selector(std::vector<T>& container) : _container(&container){};

		std::vector<int> Categorize
};

template class Selector<Site>;
