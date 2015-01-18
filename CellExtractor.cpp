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
	auto mesh = grid.toMesh();
	CellMatrix cells = getCellsFromMesh(src, mesh);

	cv::imshow("1", cells[5][6]);
	cv::imshow("2", cells[5][7]);
	cv::imshow("3", cells[6][6]);
	cv::waitKey();
	//_lineDetector->dumpLines(src, grid.getAll());
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
	auto horizontalLines = filterHorizontalLines(mesh.horizontalLines, size.width);
	std::sort(horizontalLines.begin(), horizontalLines.end(), [](Line a, Line b) -> bool {return a.start().y < b.start().y; });
	newMesh.horizontalLines = horizontalLines;

	auto verticalLines = filterVerticalLines(mesh.verticalLines, size.height);
	std::sort(verticalLines.begin(), verticalLines.end(), [](Line a, Line b) -> bool {return a.start().x < b.start().x; });
	newMesh.verticalLines = verticalLines;

	return newMesh;
}

Mesh Grid::toMesh() {
	Mesh mesh;
	for (auto horizLine : this->horizontalLines) {
		std::vector<cv::Point> columns;
		for (auto vertLine : this->verticalLines) {
			cv::Point p(vertLine.start().x, horizLine.start().y);
			columns.push_back(p);
		}

		mesh.push_back(columns);
	}

	return mesh;
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

CellMatrix CellExtractor::getCellsFromMesh(cv::Mat mat, Mesh mesh) {
	CellMatrix matrix;
	for (int r = 0; r < mesh.size() - 1; r++) {
		cv::vector<cv::Mat> columns;
		for (int c = 0; c < mesh[r].size() - 1; c++) {
			cv::Point p = mesh[r][c];
			int dx = mesh[r][c + 1].x - p.x;
			int dy = mesh[r + 1][c].y - p.y;
			cv::Mat cell = mat(cv::Rect(p.x, p.y, dx, dy));
			columns.push_back(cell);
		}

		matrix.push_back(columns);
	}

	return matrix;
}
