#include <cmath>
#include <opencv2/opencv.hpp>

#include "board_detection/board_detection.h"
#include "image_updator/image_updator.h"
#include "tip_detection/tip_detection.h"

#define deg2rad(deg) (deg*M_PI/180)

cv::Mat loadImageFromPath(std::string path)
{
    // Load image
    cv::Mat image = cv::imread("../resources/capture_l_plane.png");
	if (image.empty())
    {
		std::cerr << "Failed to open image from [" << path << "] ..." << std::endl;
	}
    return image;
}

int main(int argc, char *argv[])
{
    // Load left image
    cv::Mat left_image = loadImageFromPath("../resources/capture_l_plane.png").clone();
    // Load right image
    cv::Mat right_image = loadImageFromPath("../resources/capture_r_plane.png").clone();
    // Load front view image
    cv::Mat front_image = loadImageFromPath("../resources/front_view.jpg").clone();
    cv::blur(front_image, front_image, cv::Size(2, 2));


    /* Board detection */

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


    /* Score detection process */

    cv::Mat frame;
    cv::VideoCapture capture;
	capture.open(0); // incamera => 0, webcamera => 2
    if (capture.isOpened() == false)
    {
        // カメラが起動しないときは終了する
        std::cerr << "failed to open the video." << std::endl;
		return 0;
	}

    ImageUpdator image_updator(false);
    bool isUpdated = false;

    while (true)
    {
        capture >> frame;
        image_updator.updateFrame(frame);

        cv::namedWindow("video frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("video frame", image_updator.getFrame());
        cv::namedWindow("diff frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("diff frame", image_updator.getDiffFrame());

        if (image_updator.getUpdateFlag())
        {
            cv::namedWindow("the next sequene frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
            cv::imshow("the next sequene frame", image_updator.getNextSequenceImage());
            if (!isUpdated)
            {
                std::cout << "got it !" << std::endl;
                isUpdated = true;
            }
        }
        else
        {
            isUpdated = false;
        }

        if (cv::waitKey(1) == 'q') break; //
    }

    return 0;
}