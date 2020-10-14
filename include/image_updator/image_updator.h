#ifndef IMAGE_UPDATOR_H_
#define IMAGE_UPDATOR_H_

#include <opencv2/opencv.hpp>

class ImageUpdator
{
private:
    bool isDebug_;

    cv::Mat frame_;
    cv::Mat diff_;
    cv::Mat segm_;
    cv::Mat next_sequence_;

    cv::Ptr<cv::BackgroundSubtractor> bgfs_;

    bool flag_;
    bool isUpdated_;
    int counter_;
    int nonzero_threshold_ = 1500;

private:


public:
    ImageUpdator(const bool isDebug);
    ~ImageUpdator();

    void updateFrame(cv::Mat image);
    cv::Mat getFrame();
    cv::Mat getDiffFrame();
    cv::Mat getNextSequenceImage();
    bool getUpdateFlag();
    void setFlag(const bool flag);
};

#endif