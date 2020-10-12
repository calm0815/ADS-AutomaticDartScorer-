#include <cmath>
#include <opencv2/opencv.hpp>

#include "board_detection/board_detection.h"

#define deg2rad(deg) (deg*M_PI/180)

int main(int argc, char *argv[])
{
    cv::Mat src, result;
    cv::Mat red_mask, blue_mask, black_mask, white_mask;

    // Load source image
    src = cv::imread("../../resources/capture_l_plane.png");
	if (src.empty())
    {
		std::cerr << "Not found input image ..." << std::endl;
		return -1;
	}

    // Load front view image
    cv::Mat front_image = cv::imread("../../resources/front_view.jpg");
	if (front_image.empty())
    {
		std::cerr << "Not found front view image ..." << std::endl;
		return -1;
	}
    cv::blur(front_image, front_image, cv::Size(2, 2));


    BoardDetection board_detection(src);
    board_detection.setReferenceImage(front_image);
    board_detection.applyDetection();

    cv::namedWindow("the result of geometric transformation", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("the result of geometric transformation", board_detection.getTransformedImage());
    cv::waitKey(0);



    // Step3. 得点領域抽出 //

    // cv::warpPerspective(image, image, H, cv::Size(result.cols, result.rows));

	// // Redエリア抽出
    // red_mask = extractColor(image, cv::Scalar(0, 0, 220), cv::Scalar(190, 170, 255), 0, 1);
	// // Blueエリア抽出
    // blue_mask = extractColor(image, cv::Scalar(190, 65, 0), cv::Scalar(255, 150, 110), 0, 3);

    // // 表示
    // cv::namedWindow("red mask", cv::WINDOW_NORMAL);
    // cv::imshow("red mask", red_mask);
    // // cv::imwrite("../result/red_mask.png", red_mask);
    // cv::namedWindow("blue mask", cv::WINDOW_NORMAL);
    // cv::imshow("blue mask", blue_mask);
    // // cv::imwrite("../result/blue_mask.png", blue_mask);

    // board_mask = red_mask + blue_mask;
    // cv::morphologyEx(board_mask, board_mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), 1);
    // cv::morphologyEx(board_mask, board_mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), 5);
    // cv::dilate(board_mask, board_mask, cv::Mat(), cv::Point(-1,-1), 4);
    // board_mask = detectEllipse(board_mask, 150, 20000, cv::Scalar(0,0,255), -1);

    // for (int i=0; i<image.rows; i++)
    // {
    //     for (int j=0; j<image.cols; j++)
    //     {
    //         if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
    //     }
    // }

    // cv::imshow("target board", image);
    // cv::waitKey(0);
    // cv::imwrite("../result/all_color.png", board_mask);

    // cv::waitKey(0);
    return 0;
}