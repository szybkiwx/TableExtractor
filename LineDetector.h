#ifndef __LINE_DETECTOR__
#define __LINE_DETECTOR__
#include <vector>
#include <opencv2\core\core.hpp>
#include "Line.h"
namespace te {
	class LineDetector{
	public:
		std::vector<Line> getLinesFromImage(cv::Mat);
		void dumpLines(cv::Mat originalImage, std::vector<te::Line> lines);
	private:
		
		static const int CannyTreshold1 = 50;
		static const int CannyTreshold2 = 200;
	};
}


#endif