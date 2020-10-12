#include "board_detection/board_detection.h"

BoardDetection::BoardDetection(cv::Mat image)
:   src_(image),
    isDebug_(false)
{
    std::cout << "[BoardDetection] initialized !" << std::endl;
}

BoardDetection::~BoardDetection()
{
    std::cout << "[BoardDetection] destracted !" << std::endl;
}

void BoardDetection::applyDetection(void)
{
    board_image_ = extractBoardArea(src_);
    transform_matrix_ = calculateTransformMatrix(board_image_, transform_reference_image_);
    transformed_image_ = transformImage(src_, transform_matrix_, cv::Size(transform_reference_image_.cols, transform_reference_image_.rows));
}

cv::Mat BoardDetection::extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing)
{
    cv::Mat mask;
	cv::inRange(image, range_min, range_max, mask);

    // Opening/Closing
    if (opening != 0) cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), opening);
    if (closing != 0) cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), closing);

    return mask;
}

cv::Mat BoardDetection::detectEllipse(cv::Mat image, int threshold_min, int threshold_max, const cv::Scalar color, int thickness)
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

cv::Mat BoardDetection::extractBoardArea(cv::Mat input)
{
    cv::Mat board_image = input.clone();
    cv::Mat board_mask = input.clone();
    cv::blur(board_mask, board_mask, cv::Size(2, 2));
    board_mask = extractColor(board_mask, board_color_range_min_, board_color_range_max_, 0, 0);
    board_mask = detectEllipse(board_mask, board_detection_threshold_min_, board_detection_threshold_max_, cv::Scalar(0,0,255), -1);

    for (int i=0; i<board_image.rows; i++)
    {
        for (int j=0; j<board_image.cols; j++)
        {
            if (board_mask.at<cv::Vec3b>(i,j) != cv::Vec3b(0, 0, 255)) board_image.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
        }
    }

    return board_image;
}

cv::Mat BoardDetection::calculateTransformMatrix(cv::Mat input, cv::Mat reference_image)
{
    // Feature Detection
    std::vector<cv::KeyPoint> keypoints1;
    std::vector<cv::KeyPoint> keypoints2;
    cv::Ptr<cv::Feature2D> feature = cv::KAZE::create();

    cv::Mat desc1, desc2;

    feature->detectAndCompute(input, cv::noArray(), keypoints1, desc1);
    feature->detectAndCompute(reference_image, cv::noArray(), keypoints2, desc2);

    if (desc2.rows == 0){
        std::cerr << "WARNING: 特徴点検出できず" << std::endl;
    }

    // drawKeypoints(board_image_, keypoints1, board_image_, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);
    // drawKeypoints(reference_image, keypoints2, reference_image, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

    // Matching
    cv::BFMatcher matcher(feature->defaultNorm());
    std::vector<std::vector<cv::DMatch >> knn_matches;
    matcher.knnMatch(desc1, desc2, knn_matches, 5);

    std::vector<cv::DMatch> good_matches;

    std::vector<cv::Point2f> match_point1;
    std::vector<cv::Point2f> match_point2;

    for (size_t i = 0; i < knn_matches.size(); ++i)
    {
        auto dist1 = knn_matches[i][0].distance;
        auto dist2 = knn_matches[i][1].distance;

        //良い点を残す（最も類似する点と次に類似する点の類似度から）
        if (dist1 <= dist2 * feature_matching_threshold_)
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

    return H;
}

cv::Mat BoardDetection::transformImage(cv::Mat input, cv::Mat transform_matrix, cv::Size image_size)
{
    cv::warpPerspective(input, input, transform_matrix, image_size);
    return input;
}

cv::Mat BoardDetection::getBoardImage()
{
    return board_image_;
}

cv::Mat BoardDetection::getTransformedImage()
{
    return transformed_image_;
}

cv::Mat BoardDetection::getTransformMatrix()
{
    return transform_matrix_;
}

void BoardDetection::setBoardColorRange(cv::Scalar min, cv::Scalar max)
{
    board_color_range_min_ = min;
    board_color_range_max_ = max;
}

void BoardDetection::setBlueColorRange(cv::Scalar min, cv::Scalar max)
{
    blue_color_range_min_ = min;
    blue_color_range_max_ = max;
}

void BoardDetection::setGreenColorRange(cv::Scalar min, cv::Scalar max)
{
    green_color_range_min_ = min;
    green_color_range_max_ = max;
}

void BoardDetection::setRedColorRange(cv::Scalar min, cv::Scalar max)
{
    red_color_range_min_ = min;
    red_color_range_max_ = max;
}

void BoardDetection::setWhiteColorRange(cv::Scalar min, cv::Scalar max)
{
    white_color_range_min_ = min;
    white_color_range_max_ = max;
}

void BoardDetection::setBlackColorRange(cv::Scalar min, cv::Scalar max)
{
    black_color_range_min_ = min;
    black_color_range_max_ = max;
}

void BoardDetection::setBoardEllipseThreshold(const int min, const int max)
{
    board_detection_threshold_min_ = min;
    board_detection_threshold_max_ = max;
}

void BoardDetection::setFeatureMatchingThreshold(const float threshold)
{
    feature_matching_threshold_ = threshold;
}

void BoardDetection::setReferenceImage(cv::Mat reference_image)
{
    transform_reference_image_ = reference_image.clone();
}
