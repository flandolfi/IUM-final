//////////////////////////////////////////////////////////////////////////////////
//																				//
//									WiiMIDI										//
//								IUM - Final Term								//
//																				//
//					FILE:    Coordinate.cpp										//
//																				//
//					AUTHOR:  Francesco Landolfi (n° 444151)						//
//					EMAIL:   fran.landolfi@gmail.com 							//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "../Monitor.h"

Coordinate::Coordinate(const wxDouble& px, const wxDouble& py, const wxDouble& pz)
{
	x = px;
	y = py;
	z = pz;
}

Coordinate::Coordinate()
{
	x = 0;
	y = 0;
	z = 0;
}

Coordinate& Coordinate::operator+=(const wxDouble& d)
{
	x += d;
	y += d;
	z += d;

	return *this;
}

Coordinate& Coordinate::operator+=(const Coordinate& c)
{
	x += c.x;
	y += c.y;
	z += c.z;

	return *this;
}

Coordinate& Coordinate::operator-=(const wxDouble& d)
{
	x -= d;
	y -= d;
	z -= d;

	return *this;
}

Coordinate& Coordinate::operator-=(const Coordinate& c)
{
	x -= c.x;
	y -= c.y;
	z -= c.z;

	return *this;
}
Coordinate& Coordinate::operator*=(const wxDouble& d)
{
	x *= d;
	y *= d;
	z *= d;

	return *this;
}

Coordinate& Coordinate::operator*=(const Coordinate& c)
{
	x *= c.x;
	y *= c.y;
	z *= c.z;

	return *this;
}

Coordinate& Coordinate::operator=(const Coordinate& c)
{
	x = c.x;
	y = c.y;
	z = c.z;

	return *this;
}


Coordinate Coordinate::operator+(const Coordinate& r)
{
	Coordinate l = *this;
	l += r;

	return l;
}

Coordinate Coordinate::operator-(const Coordinate& r)
{
	Coordinate l = *this;
	l -= r;

	return l;
}

Coordinate Coordinate::operator*(const wxDouble& r)
{
	Coordinate l = *this;
	l *= r;

	return l;
}

Coordinate Coordinate::operator/(const wxDouble& r) 
{
	Coordinate l = *this;
	l *= 1.0/r;

	return l;
}