#ifndef _COLOR_DETECTION_H_
#define _COLOR_DETECTION_H_

#include <opencv2/opencv.hpp>

class ColorDetection
{
public:
    ColorDetection(cv::Mat source);
    ~ColorDetection();

    void setDetectionTargetColor(const int B=255, const int G=255, const int R=255);
    void setColorDetectionRange(cv::Scalar range=0);
    cv::Mat getMaskImage();

private:
    void generateMask();
    void opening();
    void closing();

private:
    cv::Mat source_image_;
    cv::Mat mask_image_;
    cv::Scalar detection_target_color_;
    cv::Scalar color_detection_range_;
};

#endif