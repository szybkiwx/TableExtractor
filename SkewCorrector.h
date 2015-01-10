#ifndef __SKEW_CORRECTOR__
#define __SKEW_CORRECTOR__
#include <opencv2/core/core.hpp>
#include <memory>
#include "LineDetector.h"
#include "Line.h"
namespace te {
	class SkewCorrector {
	public:
		SkewCorrector(std::shared_ptr<LineDetector> ld);
		cv::Mat rotate(cv::Mat, double angle);
		double calculateAngle(cv::Mat originalImage);
	private:
		double findBestFit(std::vector<Line> lines);
		std::shared_ptr<LineDetector> _lineDetector;
	};

}
#endif