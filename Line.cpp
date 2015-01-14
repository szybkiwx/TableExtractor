#include "stdafx.h"
#include "line.h"

using namespace te;

double Line::getLength()
{
	return sqrt(pow(diffY(), 2) + pow(diffX(), 2));
}

double Line::getAngle(AngleMeasure measure)
{
	double angle =  atan2(diffY(), diffX());
	if (measure == Degrees) {
		angle = angle * 180.f / CV_PI;
	}

	return angle;
}

double Line::diffX() {
	return (double)(*this)[2] - (double)(*this)[0];
}

double Line::diffY(){
	return (double)(*this)[3] - (double)(*this)[1];
}

cv::Point Line::start(){
	cv::Point p((*this)[0], (*this)[1]);
	return p;
}
cv::Point Line::end(){
	cv::Point p((*this)[2], (*this)[3]);
	return p;
}