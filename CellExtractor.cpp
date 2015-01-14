#include "stdafx.h"
#include "CellExtractor.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <algorithm>
using namespace te;
CellExtractor::CellExtractor(std::shared_ptr<LineDetector> ld) : _lineDetector(ld)
{
}

CellMatrix te::CellExtractor::getMatrix(cv::Mat src)
{
	CellMatrix mat;
	cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);

	cv::Mat preprocessedImage = preprocessImage(src, kernel);

	cv::Mat clearedGridImage = getClearedGridImage(preprocessedImage);
	auto lines = _lineDetector->getLinesFromImage(clearedGridImage);

	auto grid = getGrid(lines, src.size());
	_lineDetector->dumpLines(src, grid.getAll());
	return mat;
}

cv::Mat CellExtractor::floodFillCells(cv::Mat image, std::vector<Blob> blobs){

	for (auto blob : blobs) {
		cv::floodFill(image, blob.point, CV_RGB(0, 0, 0));
	}

	return image;
}

cv::Mat CellExtractor::preprocessImage(cv::Mat originalImage, cv::Mat dilationKernel) {
	int gauss = 11;
	cv::Mat destination = originalImage.clone();
	cv::GaussianBlur(originalImage, originalImage, cv::Size(gauss, gauss), 0, 0);
	cv::adaptiveThreshold(originalImage, destination, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 5, 2);
	cv::bitwise_not(destination, destination);
	cv::dilate(destination, destination, dilationKernel);
	return destination;
}

cv::Mat CellExtractor::getClearedGridImage(cv::Mat preprocessedImage) {
	auto blobs = findBlobs(preprocessedImage);
	Blob biggestBlob = blobs[0];
	blobs.erase(blobs.begin());

	cv::floodFill(preprocessedImage, biggestBlob.point, CV_RGB(255, 255, 255));

	return floodFillCells(preprocessedImage, blobs);

}
std::vector<Blob> CellExtractor::findBlobs(cv::Mat image)
{
	int count = 0;
	Blob biggest;
	biggest.area = -1;

	std::vector<Blob> blobs;

	for (int y = 0; y < image.size().height; y++)
	{
		uchar *row = image.ptr(y);
		for (int x = 0; x < image.size().width; x++)
		{
			if (row[x] > 128)
			{
				Blob blob;
				blob.point = cv::Point(x, y);
				blob.area = floodFill(image, blob.point, CV_RGB(0, 0, 64));
				blobs.push_back(blob);

				if (blob.area > biggest.area)
				{
					biggest = blob;
					std::iter_swap(blobs.begin(), blobs.end() - 1);
				}
			}
		}
	}

	return blobs;
}

Blob CellExtractor::findBiggestBlob(cv::Mat image){
	int count = 0;
	Blob blob;
	blob.area = -1;

	for (int y = 0; y < image.size().height; y++)
	{
		uchar *row = image.ptr(y);
		for (int x = 0; x < image.size().width; x++)
		{
			if (row[x] > 128)
			{
				int area = floodFill(image, cv::Point(x, y), CV_RGB(0, 0, 64));

				cv::waitKey();
				if (area > blob.area)
				{
					blob.point = cv::Point(x, y);
					blob.area = area;
				}
			}
		}

	}

	return blob;
}

Grid CellExtractor::getGrid(std::vector<Line> lines, cv::Size size)
{
	Grid mesh;
	for (auto line : lines)
	{

		double length = line.getLength();
		double angle = line.getAngle(Degrees);
		if (length > 20) {
			if (abs(angle) < 3 || 180 - abs(angle) < 3) {
				mesh.horizontalLines.push_back(line);
			}
			else if (90 - abs(angle) < 3 || 270 - abs(angle) < 3) {
				mesh.verticalLines.push_back(line);
			}
		}
	}

	return smoothenGrid(mesh, size);
}



Grid CellExtractor::smoothenGrid(Grid mesh, cv::Size size)
{
	Grid newMesh;

	newMesh.horizontalLines = filterHorizontalLines(mesh.horizontalLines, size.width);
	newMesh.verticalLines = filterVerticalLines(mesh.verticalLines, size.height);

	return newMesh;
}

std::vector<Line> CellExtractor::filterHorizontalLines(std::vector<Line> lines, int maxSize) {
	std::vector<Line> filterLines;
	std::sort(lines.begin(), lines.end(), [](Line a, Line b) -> bool { return a[1] < b[1];  });

	double lastValue = 0;
	for (auto line : lines) {
		if (lastValue < 0.000000000001 || abs(line[1] - lastValue) > 20) {
			Line copy(line);
			copy[0] = 0;
			copy[2] = maxSize;
			filterLines.push_back(copy);
		}

		lastValue = line[1];
	}

	return filterLines;
}


std::vector<Line> CellExtractor::filterVerticalLines(std::vector<Line> lines, int maxSize) {
	std::vector<Line> filterLines;
	std::sort(lines.begin(), lines.end(), [](Line a, Line b) -> bool { return a[0] < b[0];  });

	double lastValue = 0;
	for (auto line : lines) {
		if (lastValue < 0.000000000001 || abs(line[0] - lastValue) > 20) {
			Line copy(line);
			copy[1] = 0;
			copy[3] = maxSize;
			filterLines.push_back(copy);
		}

		lastValue = line[0];
	}

	return filterLines;
}
