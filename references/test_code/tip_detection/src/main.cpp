#include <opencv2/opencv.hpp>

#include "tip_detection/tip_detection.h"

// yellow
cv::Scalar yellow_min = cv::Scalar(0, 100, 100);
cv::Scalar yellow_max = cv::Scalar(70, 200, 210);

int main(int argc, char *argv[])
{
    cv::Mat image;
    cv::Mat result;

    image = cv::imread("../../../../resources/yellow.jpeg");
	if (image.empty())
    {
		std::cerr << "not found input image ..." << std::endl;
		return -1;
	}
    cv::resize(image, image, cv::Size(), 0.3, 0.3);

    TipDetection tip_detection(image, true);

    tip_detection.setYellowTipRange(yellow_min, yellow_max);
    tip_detection.applyDetection();

    cv::Mat mask = tip_detection.getTipMask();
    cv::Point position = tip_detection.getTipPosition();

    cv::waitKey(0);
    return 0;
}