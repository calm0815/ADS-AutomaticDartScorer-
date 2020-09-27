#include <opencv2/opencv.hpp>
#include <iostream>

const int B_MAX = 70;
const int B_MIN = 0;
const int G_MAX = 200;
const int G_MIN = 100;
const int R_MAX = 210;
const int R_MIN = 100;

int main(int argc, char *argv[])
{
    cv::Mat image;
    cv::Mat mask, result;

    image = cv::imread("../../resources/yellow.jpeg");
    cv::resize(image, image, cv::Size(), 0.3, 0.3);
	if (image.empty())
    {
		std::cerr << "入力画像が見つかりません" << std::endl;
		return -1;
	}

	// inRangeを用いて色抽出フィルタリング
	cv::Scalar s_min = cv::Scalar(B_MIN, G_MIN, R_MIN);
	cv::Scalar s_max = cv::Scalar(B_MAX, G_MAX, R_MAX);
	cv::inRange(image, s_min, s_max, mask);

    // マスク画像のOpening処理
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat());
    cv::threshold(mask, mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 2値化（閾値を自動で設定）

    // 表示
    cv::namedWindow("mask image", cv::WINDOW_NORMAL);
    cv::imshow("mask image", mask);
    cv::namedWindow("result", cv::WINDOW_NORMAL);
    cv::imshow("result", result);

    cv::waitKey(0);
    return 0;
}