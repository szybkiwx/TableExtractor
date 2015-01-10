#include "stdafx.h"
#include "CellExtractor.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace te;
CellExtractor::CellExtractor(std::shared_ptr<LineDetector> ld) : _lineDetector(ld)
{
}

CellMatrix te::CellExtractor::getMatrix(cv::Mat src)
{
	CellMatrix mat;



	cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);

	cv::Mat preprocessedImage = preprocessImage(src, kernel);

	Blob biggestBlob = findBiggestBlob(preprocessedImage);
	cv::Mat clearedGridImage = floodFillCells(preprocessedImage, biggestBlob);

	cv::erode(clearedGridImage, clearedGridImage, kernel);
	cv::imshow("", clearedGridImage);
	cv::waitKey(0);

	return mat;
}

cv::Mat CellExtractor::floodFillCells(cv::Mat preprocessedImage, Blob biggestBlob){
	cv::floodFill(preprocessedImage, biggestBlob.point, CV_RGB(255, 255, 255));
	cv::imshow("", preprocessedImage);
	cv::waitKey();
	for (int y = 0; y < preprocessedImage.size().height; y++)
	{
		uchar* row = preprocessedImage.ptr(y);
		for (int x = 0; x < preprocessedImage.size().width; x++)
		{
			if (row[x] == 64 && x != biggestBlob.point.x && y != biggestBlob.point.y)
			{
				int area = cv::floodFill(preprocessedImage, cv::Point(x, y), CV_RGB(0, 0, 0));
			}
		}
	}

	return preprocessedImage;
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

Mesh CellExtractor::getMesh(std::vector<Line> lines)
{
	Mesh mesh;
	for (auto line : lines)
	{

		double length = line.getLength();
		double angle = line.getAngle(Degrees);
		if (length > 300.f) {
			if (abs(angle) < 3 || 180 - abs(angle) < 3) {
				mesh.horizontalLines.push_back(line);
			}
			else if (90 - abs(angle) < 3 || 270 - abs(angle) < 3) {
				mesh.verticalLines.push_back(line);
			}
		}
	}

	return mesh;
}



Mesh CellExtractor::smoothenMesh(Mesh mesh, cv::Size size)
{
	Mesh newMesh;
	std::vector<Line> horizontalLines = mesh.horizontalLines;

	std::vector<Line> filterdHorizontalLines;
	std::sort(horizontalLines.begin(), horizontalLines.end(), [](Line a, Line b) { return a[1] < b[1];  });

	double lastValue = 0;
	for (auto line : horizontalLines) {
		if (lastValue < 0.000000000001 || abs(line[1] - lastValue) > 20) {
			Line copy(line);
			copy[0] = 0;
			copy[2] = size.width;
			filterdHorizontalLines.push_back(copy);
		}

		lastValue = line[1];
	}

	newMesh.horizontalLines = filterdHorizontalLines;
	newMesh.verticalLines = mesh.verticalLines;

	return newMesh;
}

cv::Mat CellExtractor::sobel(cv::Mat src)
{
	cv::Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	int c;

	GaussianBlur(src, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);


	/// Create window
	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Generate grad_x and grad_y
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	cv::Sobel(src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	return grad;
}

void  CellExtractor::horizDensity(cv::Mat src){

	std::vector<int> accumulators;
	int max = 0;
	for (int i = 0; i < src.rows; i++)
	{
		uint32_t acc = 0;
		for (int j = 0; j < src.cols; j++)
		{
			acc += src.at<uchar>(i, j);
			if (pow(2, 31) - acc < 100000) throw 1;
		}

		accumulators.push_back(acc);
		if (acc > max) {
			max = acc;
		}
	}

	int plot_size = 300;
	cv::Mat plot((int)accumulators.size(), plot_size, CV_8UC1);


	int i = 0;
	for (int acc : accumulators) {
		cv::line(src, cv::Point(0, i), cv::Point(plot_size * acc / max, i), cv::Scalar(255, 0, 0), 1);

		i++;
	}

	cv::resize(src, src, cv::Size(src.cols / 2, src.rows / 2));
	cv::imshow("", src);
}


