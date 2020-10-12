#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


/*************************************************
 * Mat CreateHistgram(Mat channels[3])
 * Mat channels[3] : split後のチャンネル
 *
 * 機能 : ヒストグラムを作成
 *
 * return :　image_hist (ヒストグラムのグラフ画像)
 *************************************************/
Mat CreateHistgram(Mat channels[3]){

  /* 変数宣言 */
  Mat image_hist;
  Mat R, G, B;
  int hist_size = 256;
  float range[] = {0, 256};
  const float* hist_range = range;

  /* 画素数を数える */
  calcHist(&channels[0], 1, 0, Mat(), R, 1, &hist_size, &hist_range);
  calcHist(&channels[1], 1, 0, Mat(), G, 1, &hist_size, &hist_range);
  calcHist(&channels[2], 1, 0, Mat(), B, 1, &hist_size, &hist_range);

  /* 正規化 */
  normalize(R, R, 0.0, 1.0, NORM_MINMAX, -1, Mat());
  normalize(G, G, 0.0, 1.0, NORM_MINMAX, -1, Mat());
  normalize(B, B, 0.0, 1.0, NORM_MINMAX, -1, Mat());

  /* ヒストグラム生成用の画像を作成 */
  image_hist = Mat(Size(276,320), CV_8UC3, Scalar(255,255,255));

   /* 背景を描画（見やすくするためにヒストグラム部分の背景をグレーにする） */
  for(int i = 0; i < 3; i++){
    rectangle(image_hist, Point(10, 10 + 100 * i),
              Point(265, 100 + 100 * i), Scalar(230, 230, 230), -1);
  }

  /* ヒストグラムを描画 */
  for(int i = 0; i < 256; i++){
    // それぞれのヒストグラムを描画
    line(image_hist, Point(10+i, 100),
         Point(10+i, 100-(int)(R.at<float>(i) * 80)),
         Scalar(0,0,255), 1, 8, 0);

    line(image_hist, Point(10+i, 200),
         Point(10+i, 200-(int)(G.at<float>(i) * 80)),
         Scalar(0,255,0), 1, 8, 0);

    line(image_hist, Point(10+i, 300),
         Point(10+i, 300-(int)(G.at<float>(i) * 80)),
         Scalar(255,0,0), 1, 8, 0);


    // 横軸10ずつラインを引く
    if(i%10 == 0){
      line(image_hist, Point(10+i, 100), Point(10+i, 10),
           Scalar(170,170,170), 1, 8, 0);
      line(image_hist, Point(10+i, 200), Point(10+i, 110),
           Scalar(170,170,170), 1, 8, 0);
      line(image_hist, Point(10+i, 300), Point(10+i, 210),
           Scalar(170,170,170), 1, 8, 0);

      // 横軸50ずつ濃いラインを引く
      if(i%50 == 0){
        line(image_hist, Point(10+i, 100), Point(10+i, 10),
             Scalar(50,50,50), 1, 8, 0);
        line(image_hist, Point(10+i, 200), Point(10+i, 110),
             Scalar(50,50,50), 1, 8, 0);
        line(image_hist, Point(10+i, 300), Point(10+i, 210),
             Scalar(50,50,50), 1, 8, 0);
      }
    }
  }
  return image_hist;
}

/* main関数 */
int main(int argc,char *argv[]) {

  Mat src; // 画像の読み込み変数
  Mat channels[3]; // チャンネル毎に分けた後に代入する変数

  src = imread(argv[1]); // 画像の読み込み
  imshow("source", src);  // 元画像の画像表示

  split(src, channels); // 入力画像を3チャンネルに分解
  Mat histgram = CreateHistgram(channels); // ヒストグラム作成
  imshow("histgram", histgram);  // ヒストグラム表示

  waitKey(0);
  return 0;
}