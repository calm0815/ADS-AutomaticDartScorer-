#ifndef BOARD_DETECTION_H_
#define BOARD_DETECTION_H_

#include <opencv2/opencv.hpp>

class BoardDetection
{
private:
    bool isDebug_;

    // image
    cv::Mat src_;
    cv::Mat board_image_;
    cv::Mat transform_reference_image_;
    cv::Mat transformed_image_;

    // mask image
    cv::Mat red_mask_;
    cv::Mat blue_mask_;
    cv::Mat black_mask_;
    cv::Mat white_mask_;

    // matrix
    cv::Mat transform_matrix_;

    // color range
    cv::Scalar board_color_range_min_;
    cv::Scalar board_color_range_max_;
    cv::Scalar blue_color_range_min_;
    cv::Scalar blue_color_range_max_;
    cv::Scalar green_color_range_min_;
    cv::Scalar green_color_range_max_;
    cv::Scalar red_color_range_min_;
    cv::Scalar red_color_range_max_;
    cv::Scalar white_color_range_min_;
    cv::Scalar white_color_range_max_;
    cv::Scalar black_color_range_min_;
    cv::Scalar black_color_range_max_;

    // threshold
    int board_detection_threshold_min_;
    int board_detection_threshold_max_;
    float feature_matching_threshold_;

private:
    cv::Mat extractColor(cv::Mat image, cv::Scalar range_min, cv::Scalar range_max, int opening, int closing);
    cv::Mat detectEllipse(cv::Mat image, int threshold_min, int threshold_max, const cv::Scalar color, int thickness = 1);

    cv::Mat extractBoardArea(cv::Mat input);                                                // ボード領域抽出
    cv::Mat calculateTransformMatrix(cv::Mat input, cv::Mat reference_image);               // 透視変換行列導出
    cv::Mat transformImage(cv::Mat input, cv::Mat transform_matrix, cv::Size image_size);   // 幾何変換

public:
    BoardDetection(cv::Mat image);
    ~BoardDetection();

    void applyDetection(void);

    cv::Mat getBoardImage();
    cv::Mat getTransformedImage();
    cv::Mat getTransformMatrix();

    void setBoardColorRange(const cv::Scalar min, const cv::Scalar max);
    void setBlueColorRange(const cv::Scalar min, const cv::Scalar max);
    void setGreenColorRange(const cv::Scalar min, const cv::Scalar max);
    void setRedColorRange(const cv::Scalar min, const cv::Scalar max);
    void setWhiteColorRange(const cv::Scalar min, const cv::Scalar max);
    void setBlackColorRange(const cv::Scalar min, const cv::Scalar max);
    void setBoardEllipseThreshold(const int min, const int max);
    void setFeatureMatchingThreshold(const float threshold);
    void setReferenceImage(const cv::Mat reference_image);
};

#endif  // BOARD_DETECTION_H_
