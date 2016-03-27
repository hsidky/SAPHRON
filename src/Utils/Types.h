#pragma once 

#include <Eigen/Dense>

namespace SAPHRON
{
	struct Site;

	using Vector3 = Eigen::Vector3d;
	using Matrix3 = Eigen::Matrix3d;
	using BondMatrix = Eigen::MatrixXi;
	using SiteList = std::vector<Site*>;
	using IndexList = std::vector<uint>;
}