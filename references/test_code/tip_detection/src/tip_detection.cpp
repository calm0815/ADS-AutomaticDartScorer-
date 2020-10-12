#include "tip_detection/tip_detection.h"

TipDetection::TipDetection(const cv::Mat src, const bool isDebug)
:   src_(src),
    isDebug_(isDebug)
{
    std::cout << "[TipDetection] is initialized." << std::endl;
}

TipDetection::~TipDetection()
{
    std::cout << "[TipDetection] is destracted." << std::endl;
}

void TipDetection::applyDetection()
{
    tip_mask_ = extractColor(src_, yellow_tip_color_range_min_, yellow_tip_color_range_max_, 1, 0);
    tip_position_ = estimateTipPosition(tip_mask_, "left");
}

cv::Mat TipDetection::extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing)
{
    cv::Mat mask;
	cv::inRange(image, range_min, range_max, mask);

    // Opening/Closing
    if (opening != 0) cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1,-1), opening);
    if (closing != 0) cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1,-1), closing);

    if (isDebug_)
    {
        cv::namedWindow("the result of color extraction.", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("the result of color extraction.", mask);
    }

    return mask;
}

cv::Point TipDetection::estimateTipPosition(cv::Mat mask, std::string direction)
{
    cv::Point position(mask.rows, mask.cols);

    for (int i=0; i<mask.rows; i++)
    {
        for (int j=0; j<mask.cols; j++)
        {
            if (static_cast<int>(mask.at<unsigned char>(i, j)) != 0)
            {
                // std::cout << "(row, col) = " << i << ", " << j << std::endl;
                if (j < position.x)
                {
                    position.y = i;
                    position.x = j;
                }
            }
        }
    }

    if (isDebug_)
    {
        cv::Mat marked_image = src_.clone();
        cv::circle(marked_image, position, 20, cv::Scalar(0,255,0), 2);
        cv::namedWindow("the result of tip position estimation.", cv::WINDOW_AUTOSIZE|cv::WINDOW_FREERATIO);
        cv::imshow("the result of tip position estimation.", marked_image);

        std::cout << "The coordinate of the tip of the dart is (" << position.y << ", " << position.x << ")" << std::endl;
    }

    return position;
}

void TipDetection::setYellowTipRange(cv::Scalar min, cv::Scalar max)
{
    yellow_tip_color_range_min_ = min;
    yellow_tip_color_range_max_ = max;
}

void TipDetection::setGreenTipRange(cv::Scalar min, cv::Scalar max)
{
    green_tip_color_range_min_ = min;
    green_tip_color_range_max_ = max;
}

cv::Mat TipDetection::getTipMask()
{
    return tip_mask_;
}

cv::Point TipDetection::getTipPosition()
{
    return tip_position_;
}
