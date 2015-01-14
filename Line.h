#ifndef __TE_LINE__
#define __TE_LINE__
#include <opencv2\core\core.hpp>
#include <memory>
namespace te {
	enum AngleMeasure {
		Radians,
		Degrees
	};

	

	class Line : cv::Vec4i {
	public:
		double getAngle(AngleMeasure measure = Radians);
		double getLength();
		using  cv::Vec4i::operator[];
	private:
		double diffX();
		double diffY();
		cv::Point start();
		cv::Point end();
	};

	typedef std::shared_ptr<Line> LinePtr;
}

#endif