#include "image_updator/image_updator.h"

ImageUpdator::ImageUpdator(const bool isDebug)
:   isDebug_(isDebug),
    flag_(false),
    isUpdated_(false),
    counter_(0)
{
    bgfs_ = cv::createBackgroundSubtractorMOG2();

    std::cout << "[ImageUpdator] is initialized." << std::endl;
}

ImageUpdator::~ImageUpdator()
{
    std::cout << "[ImageUpdator] is destracted." << std::endl;
}

void ImageUpdator::updateFrame(cv::Mat image)
{
    frame_ = image.clone();
    bgfs_->apply(frame_, diff_);
    cv::threshold(diff_, diff_, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 2値化（閾値を自動で設定）
    int nonzero_count = cv::countNonZero(diff_);

    if (!flag_)
    {
        if (nonzero_count < nonzero_threshold_)
        {
            if (counter_ < 50) counter_++;

            if (counter_ == 50)
            {
                if (!isUpdated_)
                {
                    next_sequence_ = frame_.clone();
                    std::cout << "cloned !" << std::endl;
                    isUpdated_ = true;
                }
            }
        }
        else if (counter_ == 50)
        {
            flag_ = true;
        }
        else
        {
            counter_ = 0;
        }
    }
    else if (flag_)
    {
        if (nonzero_count < nonzero_threshold_)
        {
            isUpdated_ = false;
            flag_ = false;
            counter_ = 0;
        }
    }

    std::cout << counter_ << " : " << flag_ << std::endl;
}

cv::Mat ImageUpdator::getFrame()
{
    return frame_;
}

cv::Mat ImageUpdator::getDiffFrame()
{
    return diff_;
}

cv::Mat ImageUpdator::getNextSequenceImage()
{
    return next_sequence_;
}

bool ImageUpdator::getUpdateFlag()
{
    return isUpdated_;
}

void ImageUpdator::setFlag(const bool flag)
{
    flag_ = flag;
}