#include <opencv2/opencv.hpp>
#include "image_updator/image_updator.h"

int main(int argc, char const *argv[])
{
    cv::VideoCapture capture;
	capture.open(0); // incamera => 0, webcamera => 2

    if (capture.isOpened() == false)
    {
        // カメラが起動しないときは終了する
        std::cerr << "failed to open the video." << std::endl;
		return 0;
	}
    ImageUpdator image_updator(false);

    cv::Mat frame;

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
        }

        if (cv::waitKey(1) == 'q') break; //
    }
    

    return 0;
}

