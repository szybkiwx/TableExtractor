#include "stdafx.h"
#include "SkewCorrector.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

te::SkewCorrector::SkewCorrector(std::shared_ptr<LineDetector> ld) : _lineDetector(ld)
{

}

cv::Mat te::SkewCorrector::rotate(cv::Mat src, double angleRadians)
{
	cv::Mat dst;
	cv::Mat rotationMatrix = cv::getRotationMatrix2D(cv::Point2f(src.cols / 2.f, src.rows / 2.f), angleRadians * 180 / CV_PI, 1);
	cv::warpAffine(src, dst, rotationMatrix, src.size());

	return dst;
}

double te::SkewCorrector::calculateAngle(cv::Mat originalImage)
{
	cv::Mat eqalized;
	cv::equalizeHist(originalImage, eqalized);
	cv::GaussianBlur(eqalized, eqalized, cv::Size(11, 11), 0);
	auto lines = _lineDetector->getLinesFromImage(eqalized);
	_lineDetector->dumpLines(originalImage, lines);
	cv::waitKey();
	return findBestFit(lines);
}

double te::SkewCorrector::findBestFit(std::vector<Line> lines)
{
	std::vector<double> angles;
	for (Line line : lines)
	{
		angles.push_back(line.getAngle());
	}

	std::sort(angles.begin(), angles.end());
	if (angles.size() % 2 == 0)
	{
		return angles[angles.size() / 2];
	}
	else
	{
		return (angles[floor(angles.size() / 2)] + angles[ceil(angles.size() / 2)]) / 2.;
	}
}


