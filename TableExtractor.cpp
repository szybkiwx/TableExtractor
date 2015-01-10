#include "stdafx.h"
#include "TableExtractor.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

te::TableExtractor::TableExtractor(
	std::shared_ptr<SkewCorrector> skewDetector, 
	std::shared_ptr<CellExtractor> cellExtractor) : _skewDetector(skewDetector), _cellExtractor(cellExtractor)
{

}

te::StringTable te::TableExtractor::processFile(std::string fileName)
{
	cv::Mat originalImage = cv::imread(fileName, 0);
	cv::Mat eqImg;
	auto angle = _skewDetector->calculateAngle(originalImage);
	cv::Mat roatedImage =_skewDetector->rotate(originalImage, angle);
	_cellExtractor->getMatrix(roatedImage);
	

	StringTable result;
	return result;
}

