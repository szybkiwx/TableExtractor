#ifndef __TABLE_EXTRACTOR__
#define __TABLE_EXTRACTOR__
#include <string>
#include <vector>
#include "SkewCorrector.h"
#include "CellExtractor.h"
#include <memory> 

namespace te {

	typedef std::vector<std::vector<std::string>> StringTable;

	class TableExtractor {
	public:
		TableExtractor(std::shared_ptr<SkewCorrector> skewDetector, std::shared_ptr<CellExtractor> cellExtractor);
		StringTable processFile(std::string fileName);
	private:
		std::shared_ptr<SkewCorrector> _skewDetector;
		std::shared_ptr<CellExtractor> _cellExtractor;
		void dumpLines(cv::Mat originalImage, cv::Mat cannyImage, std::vector<cv::Vec4i> lines);

	};
}

#endif