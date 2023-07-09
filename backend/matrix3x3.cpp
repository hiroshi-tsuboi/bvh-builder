#include "matrix3x3.h"

//     | a11 a12 a13 |
// A = | a21 a22 a23 |
//     | a31 a32 a33 |

bool Matrix3x3::inverse(Matrix3x3& result)
{
	auto& a11 = values_[0][0];
	auto& a12 = values_[0][1];
	auto& a13 = values_[0][2];

	auto& a21 = values_[1][0];
	auto& a22 = values_[1][1];
	auto& a23 = values_[1][2];

	auto& a31 = values_[2][0];
	auto& a32 = values_[2][1];
	auto& a33 = values_[2][2];

	auto detA = a11*a22*a33 + a12*a23*a32 + a13*a21*a32 - a13*a22*a31 - a11*a23*a32 - a12*a21*a33;

	if (detA == 0.f)
	{
		return false;
	}

	result.values_[0][0] = (a22*a33 - a23*a32) / detA;
       	result.values_[0][1] = (a13*a32 - a12*a33) / detA;
       	result.values_[0][2] = (a12*a23 - a13*a22) / detA;

	result.values_[1][0] = (a23*a31 - a21*a33) / detA;
	result.values_[1][1] = (a11*a33 - a13*a31) / detA;
	result.values_[1][2] = (a13*a21 - a11*a23) / detA;

	result.values_[2][0] = (a21*a32 - a22*a31) / detA;
	result.values_[2][1] = (a12*a31 - a11*a32) / detA;
	result.values_[2][2] = (a11*a22 - a12*a21) / detA;

	return true;
}
