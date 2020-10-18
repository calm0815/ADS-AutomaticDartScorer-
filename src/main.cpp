#include <cmath>
#include <time.h>
#include <opencv2/opencv.hpp>

#include "board_detection/board_detection.h"
#include "image_updator/image_updator.h"
#include "tip_detection/tip_detection.h"

#define deg2rad(deg) (deg*M_PI/180)
#define rad2deg(rad) (rad*180/M_PI)

cv::Mat loadImageFromPath(std::string path)
{
    // Load image
    cv::Mat image = cv::imread(path);
	if (image.empty())
    {
		std::cerr << "Failed to open image from [" << path << "] ..." << std::endl;
	}
    return image;
}

int calcScore(const double mag, const double angle)
{
    // detet unit
    int offset = 5;
    int num = (angle + offset) / 18;
    int score_arr[20] = {3, 17, 2, 15, 10, 6, 13, 4, 18, 1, 20, 5, 12, 9, 14, 11, 8, 16, 7, 19};
    int unit = score_arr[num];

    // detect multiple
    int multiple;
    if (mag < 237)
    {
        if (215 < mag) multiple = 2;
        else if (127 < mag && mag < 150) multiple = 3;
        else multiple = 1;
    }
    else
    {
        multiple = 0;
    }

    // score
    int score = unit * multiple;

    std::cout << "unit : " << unit << ", multiple : " << multiple << ", score : " << score << std::endl;

    return score;
}

int main(int argc, char *argv[])
{
    clock_t time;

    time = clock();

    // Load left image
    cv::Mat left_image = loadImageFromPath("../resources/capture_l_plane.png").clone();
    // Load right image
    cv::Mat right_image = loadImageFromPath("../resources/capture_r_plane.png").clone();
    // Load front view image
    cv::Mat front_image = loadImageFromPath("../resources/front_view.jpg").clone();
    cv::blur(front_image, front_image, cv::Size(2, 2));

    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;

    /* Board detection */

    BoardDetection left_board_detection(left_image, false);
    BoardDetection right_board_detection(right_image, false);

    left_board_detection.setBoardColorRange(cv::Scalar(0, 0, 0), cv::Scalar(100, 90, 60));
    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;
    left_board_detection.setBoardEllipseThreshold(1500, 2000);
    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;
    left_board_detection.setFeatureMatchingThreshold(.94f);
    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;
    left_board_detection.setReferenceImage(front_image);
    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;
    left_board_detection.applyDetection();
    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;

    right_board_detection.setBoardColorRange(cv::Scalar(0, 0, 0), cv::Scalar(100, 95, 90));
    right_board_detection.setBoardEllipseThreshold(1500, 2000);
    right_board_detection.setFeatureMatchingThreshold(.84f);
    right_board_detection.setReferenceImage(front_image);
    right_board_detection.applyDetection();

    time = clock() - time;
    std::cout << "line " << __LINE__ << " process time : " << (double)(time)/CLOCKS_PER_SEC << std::endl;

    cv::Mat left_transform_matrix = left_board_detection.getTransformMatrix();
    cv::Mat left_front_view = left_board_detection.getTransformedImage();
    cv::Mat right_transform_matrix = right_board_detection.getTransformMatrix();
    cv::Mat right_front_view = right_board_detection.getTransformedImage();


    /* Score detection process */

    cv::Mat frame;
    cv::VideoCapture capture;
	capture.open(0); // incamera => 0, webcamera => 2
    if (capture.isOpened() == false)
    {
        // カメラが起動しないときは終了する
        std::cerr << "failed to open the video." << std::endl;
		return 0;
	}

    ImageUpdator image_updator(false);
    TipDetection tip_detection(false);
    bool isUpdated = false;

    // set yellow range
    // cv::Scalar yellow_min = cv::Scalar(0, 100, 100);
    // cv::Scalar yellow_max = cv::Scalar(70, 200, 210);
    cv::Scalar yellow_min = cv::Scalar(50, 75, 100);
    cv::Scalar yellow_max = cv::Scalar(85, 105, 120);
    tip_detection.setYellowTipRange(yellow_min, yellow_max);

    cv::Mat image = left_image.clone();
    cv::Ptr<cv::BackgroundSubtractor> sub = cv::createBackgroundSubtractorMOG2();

    while (true)
    {
        capture >> frame;
        cv::resize(frame, frame, cv::Size(), left_image.rows/frame.rows, left_image.cols/frame.cols);
        image_updator.updateFrame(frame);

        cv::namedWindow("video frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("video frame", image_updator.getFrame());
        cv::namedWindow("diff frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("diff frame", image_updator.getDiffFrame());

        if (image_updator.getUpdateFlag())
        {
            cv::namedWindow("the next sequene frame", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
            cv::imshow("the next sequene frame", image_updator.getNextSequenceImage());

            if (!isUpdated)     // 画像更新時に一度だけ起動。
            {
                cv::Mat next_image = image_updator.getNextSequenceImage().clone();
                cv::Mat diff_mask;
                if (!next_image.empty()) sub->apply(next_image, diff_mask);
                cv::threshold(diff_mask, diff_mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 2値化（閾値を自動で設定）

                cv::Mat croped_image;
                cv::bitwise_and(next_image, next_image, croped_image, diff_mask);

                tip_detection.applyDetection(croped_image);
                cv::Mat mask = tip_detection.getTipMask();
                cv::Point tip_position = tip_detection.getTipPosition();

                if (tip_position.x != frame.rows && tip_position.y != frame.cols)   // ダーツ先端が検出された場合に１回動作
                {
                    cv::Point transfrmed_pos;
                    transfrmed_pos.y 
                        =   (left_transform_matrix.at<double>(0,0)*tip_position.x + left_transform_matrix.at<double>(0,1)*tip_position.y + left_transform_matrix.at<double>(0,2)) / 
                            (left_transform_matrix.at<double>(2,0)*tip_position.x + left_transform_matrix.at<double>(2,1)*tip_position.y + left_transform_matrix.at<double>(2,2));
                    transfrmed_pos.x 
                        =   (left_transform_matrix.at<double>(1,0)*tip_position.x + left_transform_matrix.at<double>(1,1)*tip_position.y + left_transform_matrix.at<double>(1,2)) / 
                            (left_transform_matrix.at<double>(2,0)*tip_position.x + left_transform_matrix.at<double>(2,1)*tip_position.y + left_transform_matrix.at<double>(2,2));

                    cv::Point origin_pos = cv::Point(300, 317);
                    cv::Point fixed_pos = transfrmed_pos - origin_pos;

                    // 2次元座標から，大きさと角度を求める．
                    cv::Mat x = (cv::Mat_<double>(1,1) << fixed_pos.x);
                    cv::Mat y = (cv::Mat_<double>(1,1) << fixed_pos.y);
                    cv::Mat magnitude, angle;
                    cv::cartToPolar(x, y, magnitude, angle, true); // in degrees
                    std::cout << "magnitude : " << magnitude << ", angle : " << angle << std::endl;

                    int score = calcScore(magnitude.at<double>(0,0), angle.at<double>(0,0));
                    std::cout << "score : " << score << std::endl;

                    // 表示
                    cv::Mat draw_board = left_image.clone();
                    cv::circle(draw_board, tip_position, 10, cv::Scalar(0,255,0), 2);
                    cv::Mat result_draw_image = left_front_view.clone();
                    cv::circle(result_draw_image, cv::Point(transfrmed_pos.y, transfrmed_pos.x), 5, cv::Scalar(0,255,0), -1);
                    cv::namedWindow("detected tip position", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
                    cv::imshow("detected tip position", draw_board);
                    cv::namedWindow("estimated tip position", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
                    cv::imshow("estimated tip position", result_draw_image);
                }

                isUpdated = true;
            }
        }
        else
        {
            isUpdated = false;
        }

        if (cv::waitKey(1) == 'q') break; //
    }



    return 0;
}