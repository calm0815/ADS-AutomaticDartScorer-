#include <cmath>
#include <opencv2/opencv.hpp>

// #include <opencv2/core/core.hpp>
// #include "opencv2/highgui/highgui.hpp"
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/features2d/features2d.hpp>

#define deg2rad(deg) (deg*M_PI/180)

cv::Mat extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing)
{
    cv::Mat mask;
	cv::inRange(image, range_min, range_max, mask);

    // Opening/Closing
    if (opening != 0) cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), opening);
    if (closing != 0) cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), closing);

    return mask;
}

cv::Mat detectEllipse(cv::Mat image, int threshold_min, int threshold_max, const cv::Scalar color, int thickness = 1)
{
    std::vector<std::vector<cv::Point>> detected_contours;

    cv::findContours(image, detected_contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);     // 楕円切り出し
    if (image.type() == 0) cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

    for (int i = 0; i < detected_contours.size(); ++i)
    {
        size_t count = detected_contours[i].size();
        if(count < threshold_min || count > threshold_max) continue;                    // （小さすぎる|大きすぎる）輪郭を除外
        cv::Mat pointsf;
        cv::Mat(detected_contours[i]).convertTo(pointsf, CV_32F);
        cv::RotatedRect box = cv::fitEllipse(pointsf);                  // 楕円フィッティング
        cv::ellipse(image, box, color, thickness, cv::LINE_AA);   // 楕円の描画
    }

    return image;
}

int main(int argc, char *argv[])
{
    cv::Mat src, result;
    cv::Mat red_mask, blue_mask, black_mask, white_mask;
    cv::Mat edges;

    // Load source image
    src = cv::imread("../../resources/capture_l_plane.png");
	if (src.empty())
    {
		std::cerr << "Not found input image ..." << std::endl;
		return -1;
	}

    cv::Mat image = src.clone();

    cv::namedWindow("source image", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("source image", src);
    // cv::waitKey(0);

    // Step.1 対象領域の切り出し //

    // ボード領域抽出
    cv::Mat board_image = image.clone();
    cv::Mat board_mask = image.clone();
    cv::blur(board_mask, board_mask, cv::Size(2, 2));
    board_mask = extractColor(board_mask, cv::Scalar(0, 0, 0), cv::Scalar(100, 90, 60), 0, 0);
    board_mask = detectEllipse(board_mask, 1500, 2000, cv::Scalar(0,0,255), -1);

    for (int i=0; i<board_image.rows; i++)
    {
        for (int j=0; j<board_image.cols; j++)
        {
            if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) board_image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
        }
    }

    cv::namedWindow("board extraction", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("board extraction", board_image);
    // cv::waitKey(0);


    // Step.2 幾何変換 //
    
    // Load front view image
    cv::Mat front_image = cv::imread("../../resources/front_view.jpg");
	if (front_image.empty())
    {
		std::cerr << "Not found front view image ..." << std::endl;
		return -1;
	}
    cv::blur(front_image, front_image, cv::Size(2, 2));

    // Feature Detection
    std::vector<cv::KeyPoint> keypoints1;
    std::vector<cv::KeyPoint> keypoints2;
    cv::Ptr<cv::Feature2D> feature = cv::KAZE::create();

    cv::Mat desc1, desc2;

    feature->detectAndCompute(board_image, cv::noArray(), keypoints1, desc1);
    feature->detectAndCompute(front_image, cv::noArray(), keypoints2, desc2);

    if (desc2.rows == 0){
        std::cerr << "WARNING: 特徴点検出できず" << std::endl;
        return -1;
    }

    drawKeypoints(board_image, keypoints1, board_image, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);
    drawKeypoints(front_image, keypoints2, front_image, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

    cv::namedWindow("keypoints for [source image]", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("keypoints for [source image]", board_image);
    // cv::waitKey(0);

    cv::namedWindow("keypoints for [front view image]", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
    cv::imshow("keypoints for [front view image]", front_image);
    // cv::waitKey(0);

    // Matching
    cv::BFMatcher matcher(feature->defaultNorm());
    std::vector<std::vector<cv::DMatch >> knn_matches;
    matcher.knnMatch(desc1, desc2, knn_matches, 5);

    const auto match_par = .94f; //対応点のしきい値
    std::vector<cv::DMatch> good_matches;

    std::vector<cv::Point2f> match_point1;
    std::vector<cv::Point2f> match_point2;

    for (size_t i = 0; i < knn_matches.size(); ++i)
    {
        auto dist1 = knn_matches[i][0].distance;
        auto dist2 = knn_matches[i][1].distance;

        //良い点を残す（最も類似する点と次に類似する点の類似度から）
        if (dist1 <= dist2 * match_par)
        {
            good_matches.push_back(knn_matches[i][0]);
            match_point1.push_back(keypoints1[knn_matches[i][0].queryIdx].pt);
            match_point2.push_back(keypoints2[knn_matches[i][0].trainIdx].pt);
        }
    }

    std::cout << match_point1.size() << std::endl;
    std::cout << match_point2.size() << std::endl;

    //ホモグラフィ行列推定
    cv::Mat masks;
    cv::Mat H;
    if (match_point1.size() != 0 && match_point2.size() != 0)
    {
        H = cv::findHomography(match_point1, match_point2, cv::RANSAC);
    }

    std::cout << H.size() << std::endl;

    result = src.clone();
    cv::warpPerspective(result, result, H, cv::Size(front_image.cols, front_image.rows));

    cv::imshow("the result of geometric transformation", result);
    cv::waitKey(0);


    // Step3. 得点領域抽出 //

    // cv::warpPerspective(image, image, H, cv::Size(result.cols, result.rows));

	// // Redエリア抽出
    // red_mask = extractColor(image, cv::Scalar(0, 0, 220), cv::Scalar(190, 170, 255), 0, 1);
	// // Blueエリア抽出
    // blue_mask = extractColor(image, cv::Scalar(190, 65, 0), cv::Scalar(255, 150, 110), 0, 3);

    // // 表示
    // cv::namedWindow("red mask", cv::WINDOW_NORMAL);
    // cv::imshow("red mask", red_mask);
    // // cv::imwrite("../result/red_mask.png", red_mask);
    // cv::namedWindow("blue mask", cv::WINDOW_NORMAL);
    // cv::imshow("blue mask", blue_mask);
    // // cv::imwrite("../result/blue_mask.png", blue_mask);

    // board_mask = red_mask + blue_mask;
    // cv::morphologyEx(board_mask, board_mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), 1);
    // cv::morphologyEx(board_mask, board_mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), 5);
    // cv::dilate(board_mask, board_mask, cv::Mat(), cv::Point(-1,-1), 4);
    // board_mask = detectEllipse(board_mask, 150, 20000, cv::Scalar(0,0,255), -1);

    // for (int i=0; i<image.rows; i++)
    // {
    //     for (int j=0; j<image.cols; j++)
    //     {
    //         if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
    //     }
    // }

    // cv::imshow("target board", image);
    // cv::waitKey(0);
    // cv::imwrite("../result/all_color.png", board_mask);

    // cv::waitKey(0);
    return 0;
}
