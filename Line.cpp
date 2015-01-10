#include "stdafx.h"
#include "line.h"

double te::Line::getLength()
{
	return sqrt(pow(diffY(), 2) + pow(diffX(), 2));
}

double te::Line::getAngle(AngleMeasure measure)
{
	double angle =  atan2(diffY(), diffX());
	if (measure == Degrees) {
		angle = angle * 180.f / CV_PI;
	}

	return angle;
}

double te::Line::diffX() {
	return (double)(*this)[2] - (double)(*this)[0];
}

double te::Line::diffY(){
	return (double)(*this)[3] - (double)(*this)[1];
}