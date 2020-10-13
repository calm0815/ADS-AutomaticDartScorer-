#include <opencv2/opencv.hpp>

#include "tip_detection/tip_detection.h"

int main(int argc, char *argv[])
{
    bool isCamera = false;

        // カメラからの動画を取り込むためのオブジェクトを宣言する
    cv::VideoCapture capture;
    cv::Mat image;
    cv::Mat result;

    image = cv::imread("../../../../resources/yellow.jpeg");
	if (image.empty())
    {
		std::cerr << "not found input image ..." << std::endl;
		return -1;
	}
    cv::resize(image, image, cv::Size(), 0.3, 0.3);

    if (isCamera)
    {
        // カメラを起動する
        capture.open(0); // incamera => 0, webcamera => 2

        if (capture.isOpened() == false)
        {
            // カメラが起動しないときは終了する
            std::cerr << "failed to open the video." << std::endl;
            return 0;
        }
        capture >> image; //1フレームの画像の読み込み(グレー化)
    }

    TipDetection tip_detection(image, true);

    // yellow
    cv::Scalar yellow_min = cv::Scalar(0, 100, 100);
    cv::Scalar yellow_max = cv::Scalar(70, 200, 210);

    tip_detection.setYellowTipRange(yellow_min, yellow_max);

    while (!image.empty())
    {
        tip_detection.applyDetection();
        cv::Mat mask = tip_detection.getTipMask();
        cv::Point position = tip_detection.getTipPosition();

        // 表示
        cv::namedWindow("result image", cv::WINDOW_NORMAL);
        cv::imshow("result image", tip_detection.getTipMask());

        if (!isCamera) break;
        capture >> image;
        tip_detection.updateSourceImage(image);
    }

    cv::waitKey(0);
    return 0;
}