#include <cmath>
#include <opencv2/opencv.hpp>


int main(int argc, char *argv[])
{
    cv::Mat image;
    cv::Mat result;


    // Load source image
    image = cv::imread("../../resources/plane.jpeg");
    cv::resize(image, image, cv::Size(), 0.6, 0.6);
    result = image.clone();


    // Canny edge
    cv::Mat edges;
    cv::blur(image, image, cv::Size(3, 3)); 
    cv::Canny(image, edges, 200, 600, 3);

    cv::namedWindow("Canny result", cv::WINDOW_NORMAL);
    cv::imshow("Canny result", edges);


    // Voting
    int diagonal = std::hypot(edges.rows, edges.cols);  // 平方根
    cv::Mat accumlate = cv::Mat::zeros(2*diagonal, 180, CV_8UC1);
    // cv::Mat accumlate = cv::Mat::zeros(2*diagonal, 180, CV_8UC3);

    for (int x=0; x<edges.cols; x++)
    {
        for (int y=0; y<edges.rows; y++)
        {
            if (edges.at<uchar>(y, x) == 255)
            {
                for (int theta=0; theta<180; theta++)
                {
                    double rho = round( y*sin(theta*M_PI/180) + x*cos(theta*M_PI/180) ) + diagonal;
                    accumlate.at<uchar>(rho, theta)+=2;
                    // accumlate.at<cv::Vec3b>(rho, theta)[2]++;
                }
            }
        }
    }
    // cv::resize(accumlate, accumlate, cv::Size(), 0.3*accumlate.rows/accumlate.cols, 0.3);   // 確認用の縦横比調整


    // Sorting
    std::vector<std::vector<int>> votes;
    std::vector<int> tmp;
    for (int rho=0; rho<accumlate.rows; rho++)
    {
        for (int theta=0; theta<accumlate.cols; theta++)
        {
            tmp.clear();
            tmp.push_back(accumlate.at<uchar>(rho, theta)); tmp.push_back(rho); tmp.push_back(theta);
            votes.push_back(tmp);
        }
    }
    std::sort(votes.begin(),votes.end(),[](const std::vector<int> &alpha,const std::vector<int> &beta){return alpha[0] > beta[0];});

    // Check
    for (int i=0; i<15; i++)
    {
        std::cout << "(vote, rho, theta) : " << votes[i][0] << "," << votes[i][1] << "," << votes[i][2] << std::endl;
        cv::circle(accumlate, cv::Point(votes[i][2],votes[i][1]), 15, 255, 1);
    }
    // cv::namedWindow("Accumlation result", cv::WINDOW_NORMAL);
    // cv::imshow("Accumlation result", accumlate);
    cv::imwrite("../result/hough_result.png", accumlate);


    // Polar to Cartesian
    std::vector<cv::Point> coordinate;
    coordinate.push_back(cv::Point(10,20));
    coordinate.push_back(cv::Point(100,100));

    cv::line(result, coordinate[0], coordinate[1], cv::Scalar(0,255,0), 1, cv::LINE_AA);

    cv::namedWindow("result", cv::WINDOW_NORMAL);
    cv::imshow("result", result);

    cv::waitKey(0);
    return 0;
}
