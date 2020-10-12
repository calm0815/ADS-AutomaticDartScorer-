#ifndef TIP_DETECTION_H_
#define TIP_DETECTION_H_

#include <opencv2/opencv.hpp>

class TipDetection
{
private:
    bool isDebug_;

    // image
    cv::Mat src_;
    cv::Mat tip_mask_;

    // color range
    cv::Scalar yellow_tip_color_range_min_;
    cv::Scalar yellow_tip_color_range_max_;
    cv::Scalar green_tip_color_range_min_;
    cv::Scalar green_tip_color_range_max_;

    // tip coordinate
    cv::Point tip_position_;

private:
    cv::Mat extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing);
    cv::Point estimateTipPosition(cv::Mat mask, std::string direction);

public:
    TipDetection(const cv::Mat src, const bool isDebug);
    ~TipDetection();

    void applyDetection();

    void setYellowTipRange(cv::Scalar min, cv::Scalar max);
    void setGreenTipRange(cv::Scalar min, cv::Scalar max);
    cv::Mat getTipMask();
    cv::Point getTipPosition();
};

#endif  // TIP_DETECTION_H_