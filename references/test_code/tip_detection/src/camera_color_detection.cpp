#include <opencv2/opencv.hpp>
#include <iostream>

// yellow
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

    capture >> image; //1フレームの画像の読み込み(グレー化)

    while (true)
    {
        // inRangeを用いて色抽出フィルタリング
        cv::Scalar s_min = cv::Scalar(B_MIN, G_MIN, R_MIN);
        cv::Scalar s_max = cv::Scalar(B_MAX, G_MAX, R_MAX);
        cv::inRange(image, s_min, s_max, mask);

        // マスク画像のOpening処理
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat());
        cv::threshold(mask, mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 2値化（閾値を自動で設定）

        std::vector<int> point(mask.rows, mask.cols);

        for (int i=0; i<mask.rows; i++)
        {
            for (int j=0; j<mask.cols; j++)
            {
                if (static_cast<int>(mask.at<unsigned char>(i, j)) != 0)
                {
                    // std::cout << "(row, col) = " << i << ", " << j << std::endl;
                    if (j < point[1])
                    {
                        point[0] = i;
                        point[1] = j;
                    }
                }
            }
        }

        std::cout << "The coordinate of the tip of the dart is (" << point[0] << ", " << point[1] << ")" << std::endl;

        // マーキング
        cv::circle(image, cv::Point(point[1],point[0]), 20, cv::Scalar(0,255,0), 2);

        // 表示
        cv::namedWindow("mask image", cv::WINDOW_NORMAL);
        cv::imshow("mask image", mask);
        cv::namedWindow("result image", cv::WINDOW_NORMAL);
        cv::imshow("result image", image);

        capture >> image; //画像の読み込み 
        if (cv::waitKey(1) == 'q') break; //
    }


    return 0;
}