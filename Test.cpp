#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "SkewCorrector.h"
#include "TableExtractor.h"
#include "CellExtractor.h"
using namespace te;
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << " Usage: display_image ImageToLoadAndDisplay" << std::endl;
		return -1;
	}
	std::shared_ptr<LineDetector> ld;
	
	std::shared_ptr<SkewCorrector> psc(new SkewCorrector(ld));
	std::shared_ptr<CellExtractor> pce(new CellExtractor(ld));

	TableExtractor t(psc, pce);
	t.processFile(argv[1]);
	//s.detect(argv[1]);



	return 0;
}