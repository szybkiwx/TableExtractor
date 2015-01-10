#include "stdafx.h"
#include "LineDetector.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "Line.h"

using namespace te;

std::vector<Line> LineDetector::getLinesFromImage(cv::Mat originalImage)
{
	cv::Mat cannyImage;
	//cv::GaussianBlur(originalImage, cannyImage, cv::Size(5, 5), 0);
	cv::Canny(originalImage, cannyImage, CannyTreshold1, CannyTreshold2);
	std::vector<cv::Vec4i> lines;
	HoughLinesP(cannyImage, lines, 1, CV_PI / 180, 100, 100, 20);
	
	std::vector<Line> result;
	for (auto line : lines) {
		result.push_back(*(Line*)&line);
	}

	return result;
}

void LineDetector::dumpLines(cv::Mat originalImage, std::vector<Line> lines)
{
	cv::Mat cdst = originalImage.clone();
	//cv::cvtColor(cannyImage, cdst, CV_GRAY2BGR);

	for (Line l : lines)
	{
		line(cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 124, 255), 3, CV_AA);
	}
	float factor = 4.f;

	cv::resize(cdst, cdst, cv::Size(cdst.cols / factor, cdst.rows / factor));
	cv::imshow("source.jpg", cdst);
	cv::waitKey();
}