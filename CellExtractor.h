#ifndef __CELL_EXTRACTOR__
#define __CELL_EXTRACTOR__
#include <vector>
#include <opencv2/core/core.hpp>
#include <memory>
#include "LineDetector.h"
#include "Line.h"

namespace te {
	typedef std::vector<std::vector<cv::Mat>> CellMatrix;
	
	struct Mesh {
		std::vector<Line> horizontalLines;
		std::vector<Line> verticalLines;

		std::vector<Line> getAll(){
			std::vector<Line> result = horizontalLines;
			result.insert(result.end(), verticalLines.begin(), verticalLines.end());
			return result;
		}
	};

	struct Blob {
		cv::Point point;
		int area;
	};

	class CellExtractor {
	public:
		CellExtractor(std::shared_ptr<LineDetector>);
		CellMatrix getMatrix(cv::Mat src);
		Mesh getMesh(std::vector<Line> lines);
		Mesh smoothenMesh(Mesh mesh, cv::Size size);
		cv::Mat sobel(cv::Mat src);
		void horizDensity(cv::Mat src);
		Blob findBiggestBlob(cv::Mat image);
		cv::Mat preprocessImage(cv::Mat originalImage, cv::Mat dilationKernel);
		cv::Mat floodFillCells(cv::Mat image, Blob biggestBlob);
	private:
		std::shared_ptr<LineDetector> _lineDetector;
		
	};
}


#endif