#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
  cv::Mat buf, frame, diff;
  // カメラからの動画を取り込むためのオブジェクトを宣言する
  cv::VideoCapture capture;

  cv::Ptr<cv::BackgroundSubtractorMOG2> pMOG2;
  pMOG2 = cv::createBackgroundSubtractorMOG2(5, 100, false);

  // // カメラを起動する
	// capture.open(0);
	
  // 動画ファイルのパスの文字列を格納するオブジェクトを宣言する
  std::string filepath = "../resources/play_0.mov";
	capture.open(filepath);

  if (capture.isOpened() == false)
  {
		// カメラが起動しないときは終了する
    std::cout << "failed to open the video." << std::endl;
		return 0;
	}

  capture >> buf;

  while (!buf.empty())
  {
    cv::Mat foreGroundMask;
    pMOG2->apply(buf, foreGroundMask);

    cv::imshow("test", foreGroundMask);
    if (cv::waitKey(1) == 0x1b) break;  //ESCキー

    capture >> buf;
  }

	return 0;  
}