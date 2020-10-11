#include <cmath>
#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#define deg2rad(deg) (deg*M_PI/180)

cv::Mat extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing)
{
    cv::Mat mask;
	cv::inRange(image, range_min, range_max, mask);

    // Opening/Closing
    if (opening != 0) cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), opening);
    if (closing != 0) cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), closing);

    return mask;
}

cv::Mat detectEllipse(cv::Mat image, int threshold_min, int threshold_max, const cv::Scalar color, int thickness = 1)
{
    std::vector<std::vector<cv::Point>> detected_contours;

    cv::findContours(image, detected_contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);     // 楕円切り出し
    if (image.type() == 0) cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

    for (int i = 0; i < detected_contours.size(); ++i)
    {
        size_t count = detected_contours[i].size();
        if(count < threshold_min || count > threshold_max) continue;                    // （小さすぎる|大きすぎる）輪郭を除外
        cv::Mat pointsf;
        cv::Mat(detected_contours[i]).convertTo(pointsf, CV_32F);
        cv::RotatedRect box = cv::fitEllipse(pointsf);                  // 楕円フィッティング
        cv::ellipse(image, box, color, thickness, cv::LINE_AA);   // 楕円の描画
    }

    return image;
}

int main(int argc, char *argv[])
{
    cv::Mat src, result;
    cv::Mat board_mask, red_mask, blue_mask, black_mask, white_mask;
    cv::Mat edges;

    // Load source image
    src = cv::imread("../../resources/capture_l_plane.png");
	if (src.empty())
    {
		std::cerr << "Not found input image..." << std::endl;
		return -1;
	}

    cv::Mat image = src.clone();

    // Step.1 対象領域の切り出し //

    // ボード領域抽出
    board_mask = src.clone();
    cv::blur(board_mask, board_mask, cv::Size(2, 2));
    board_mask = extractColor(board_mask, cv::Scalar(0, 0, 0), cv::Scalar(100, 90, 60), 0, 0);
    board_mask = detectEllipse(board_mask, 1500, 2000, cv::Scalar(0,0,255), -1);

    for (int i=0; i<image.rows; i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
        }
    }

	// Redエリア抽出
    red_mask = extractColor(image, cv::Scalar(0, 0, 220), cv::Scalar(190, 170, 255), 0, 1);
	// Blueエリア抽出
    blue_mask = extractColor(image, cv::Scalar(190, 65, 0), cv::Scalar(255, 150, 110), 0, 3);

    // 表示
    // cv::namedWindow("red mask", cv::WINDOW_NORMAL);
    // cv::imshow("red mask", red_mask);
    // cv::imwrite("../result/red_mask.png", red_mask);
    // cv::namedWindow("blue mask", cv::WINDOW_NORMAL);
    // cv::imshow("blue mask", blue_mask);
    // cv::imwrite("../result/blue_mask.png", blue_mask);

    board_mask = red_mask + blue_mask;
    cv::morphologyEx(board_mask, board_mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), 1);
    cv::morphologyEx(board_mask, board_mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), 5);
    cv::dilate(board_mask, board_mask, cv::Mat(), cv::Point(-1,-1), 4);
    board_mask = detectEllipse(board_mask, 150, 20000, cv::Scalar(0,0,255), -1);

    for (int i=0; i<image.rows; i++)
    {
        for (int j=0; j<image.cols; j++)
        {
            if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
        }
    }

    cv::namedWindow("target board", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("target board", image);
    cv::imwrite("../result/all_color.png", board_mask);


    // Step.2 幾何変換 //

    // FeatureDetector
    cv::Ptr<cv::FeatureDetector> featureDetector;
    cv::Ptr<cv::DescriptorExtractor> descriptorExtractor;
    featureDetector = cv::FeatureDetector::create("SURF");
    descriptorExtractor = cv::DescriptorExtractor::create("SURF");



    cv::waitKey(0);
    return 0;
}
