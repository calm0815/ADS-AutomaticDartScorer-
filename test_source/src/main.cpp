#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
  cv::Mat gray_img1,gray_img2,gray_img3,diff1,diff2,frame,diff;
    // カメラからの動画を取り込むためのオブジェクトを宣言する
  cv::VideoCapture capture;
	
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

  capture >> frame; //1フレームの画像の読み込み(グレー化)
  cv::cvtColor(frame, gray_img1, cv::COLOR_BGR2GRAY);
  cv::cvtColor(frame, gray_img2, cv::COLOR_BGR2GRAY);
  cv::cvtColor(frame, gray_img3, cv::COLOR_BGR2GRAY);

  while (1)
  {
    //差分1：フレーム1と2の差を求める
    cv::absdiff(gray_img1, gray_img2,diff1);
		//差分2：フレーム2と3の差を求める
		cv::absdiff(gray_img2, gray_img3,diff2);
		//差分1と差分2の結果を比較(論理積)し、diffに出力
		cv::bitwise_and(diff1,diff2,diff);
    //表示
    cv::namedWindow("diff image", cv::WINDOW_FULLSCREEN);
    cv::namedWindow("gray image", cv::WINDOW_FULLSCREEN);
    cv::imshow("diff image", diff);
    cv::imshow("gray image", gray_img1);
    //画像を1frameずらす
    gray_img2.copyTo(gray_img1,gray_img2);
    gray_img3.copyTo(gray_img2,gray_img3);
    capture >> frame; //画像の読み込み
    cv::cvtColor(frame, gray_img3, cv::COLOR_BGR2GRAY); 
    if (cv::waitKey(1) == 'q') break; //
    if (diff.empty() == true) break;  // 画像が読み込めなかったとき、無限ループを抜ける
	}
	return 0;  
}