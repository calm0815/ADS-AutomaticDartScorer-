#include <cmath>
#include <opencv2/opencv.hpp>

#include "board_detection/board_detection.h"

#define deg2rad(deg) (deg*M_PI/180)

int main(int argc, char *argv[])
{
    // Load left image
    cv::Mat left_image = cv::imread("../resources/capture_l_plane.png");
	if (left_image.empty())
    {
		std::cerr << "Not found input image ..." << std::endl;
		return -1;
	}
    // Load right image
    cv::Mat right_image = cv::imread("../resources/capture_r_plane.png");
	if (right_image.empty())
    {
		std::cerr << "Not found input image ..." << std::endl;
		return -1;
	}

    // Load front view image
    cv::Mat front_image = cv::imread("../resources/front_view.jpg");
	if (front_image.empty())
    {
		std::cerr << "Not found front view image ..." << std::endl;
		return -1;
	}
    cv::blur(front_image, front_image, cv::Size(2, 2));


    BoardDetection left_board_detection(left_image, false);
    BoardDetection right_board_detection(right_image, false);

    left_board_detection.setBoardColorRange(cv::Scalar(0, 0, 0), cv::Scalar(100, 90, 60));
    left_board_detection.setBoardEllipseThreshold(1500, 2000);
    left_board_detection.setFeatureMatchingThreshold(.94f);
    left_board_detection.setReferenceImage(front_image);
    left_board_detection.applyDetection();

    right_board_detection.setBoardColorRange(cv::Scalar(0, 0, 0), cv::Scalar(100, 95, 90));
    right_board_detection.setBoardEllipseThreshold(1500, 2000);
    right_board_detection.setFeatureMatchingThreshold(.84f);
    right_board_detection.setReferenceImage(front_image);
    right_board_detection.applyDetection();

    cv::Mat left_transform_matrix = left_board_detection.getTransformMatrix();
    cv::Mat left_front_view = left_board_detection.getTransformedImage();
    cv::Mat right_transform_matrix = right_board_detection.getTransformMatrix();
    cv::Mat right_front_view = right_board_detection.getTransformedImage();

    

    cv::waitKey(0);
    return 0;
}