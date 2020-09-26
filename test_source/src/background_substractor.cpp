#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    cv::Mat front_img, back_img;
    cv::Mat gray_img1, gray_img2,diff;

    front_img = cv::imread("../resources/substract_test0.png");
    back_img  = cv::imread("../resources/substract_test1.png");

    cv::cvtColor(front_img, gray_img1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(back_img,  gray_img2, cv::COLOR_BGR2GRAY);

    //差分1：フレーム1と2の差を求める
    cv::absdiff(gray_img1, gray_img2, diff);
    //表示
    cv::namedWindow("diff image", cv::WINDOW_FULLSCREEN);
    cv::imshow("diff image", diff);

    cv::waitKey(0);
    return 0;
}