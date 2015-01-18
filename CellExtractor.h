#ifndef __CELL_EXTRACTOR__
#define __CELL_EXTRACTOR__
#include <vector>
#include <opencv2/core/core.hpp>
#include <memory>
#include "LineDetector.h"
#include "Line.h"

namespace te {
	typedef std::vector<std::vector<cv::Mat>> CellMatrix;
	
	typedef std::vector<std::vector<cv::Point>> Mesh;

	struct Grid {
		std::vector<Line> horizontalLines;
		std::vector<Line> verticalLines;

		std::vector<Line> getAll(){
			std::vector<Line> result = horizontalLines;
			result.insert(result.end(), verticalLines.begin(), verticalLines.end());
			return result;
		};
		
		Mesh toMesh();
	};


	struct Blob {
		cv::Point point;
		int area;
	};

	class CellExtractor {
	public:
		CellExtractor(std::shared_ptr<LineDetector>);
		CellMatrix getMatrix(cv::Mat src);

	private:
		std::shared_ptr<LineDetector> _lineDetector;

		Grid getGrid(std::vector<Line> lines, cv::Size size);
		Blob findBiggestBlob(cv::Mat image);
		cv::Mat preprocessImage(cv::Mat originalImage, cv::Mat dilationKernel);
		cv::Mat getClearedGridImage(cv::Mat preprocessedImage);
		cv::Mat floodFillCells(cv::Mat image, std::vector<Blob> blobs);
		std::vector<Blob> findBlobs(cv::Mat image);
		std::vector<Line> CellExtractor::filterHorizontalLines(std::vector<Line> lines, int maxSize);
		std::vector<Line> CellExtractor::filterVerticalLines(std::vector<Line> lines, int maxSize);
		Grid smoothenGrid(Grid mesh, cv::Size size);
		CellMatrix getCellsFromMesh(cv::Mat mat, Mesh mesh);
		
	};
}


#endif