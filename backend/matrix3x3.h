#pragma once

struct Matrix3x3
{
	double values_[3][3];

	bool inverse(Matrix3x3& result);
};
