#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    cv::Mat image;

    // カメラからの動画を取り込むためのオブジェクトを宣言する
    cv::VideoCapture capture;

    // カメラを起動する
	capture.open(2); // incamera => 0, webcamera => 2

    if (capture.isOpened() == false)
    {
        // カメラが起動しないときは終了する
        std::cerr << "failed to open the video." << std::endl;
		return 0;
	}

    cv::namedWindow("capture", cv::WINDOW_NORMAL);

    while (true)
    {
        capture >> image; //1フレームの画像の読み込み(グレー化)
        cv::imshow("capture", image);

        if (cv::waitKey(1) == 'r') cv::imwrite("../result/capture.png", image);
        if (cv::waitKey(1) == 'q') break;
    }

    return 0;
}
