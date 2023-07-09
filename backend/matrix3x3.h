#pragma once

struct Matrix3x3
{
	float values_[3][3];

	bool inverse(Matrix3x3& result);
};
