#include <cmath>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    cv::Mat image;
    cv::Mat result;

    image = cv::imread("../../resources/plane.jpeg");
    cv::resize(image, image, cv::Size(), 0.5, 0.5);

    // エッジ検出
    cv::Mat edges;
    cv::blur(image, image, cv::Size(3, 3)); 
    cv::Canny(image, edges, 100, 300, 3);

    cv::namedWindow("Canny result", cv::WINDOW_NORMAL);
    cv::imshow("Canny result", edges);

    // Vote
    int diagonal = std::hypot(edges.rows, edges.cols);  // 平方根
    cv::Mat accumlate = cv::Mat::zeros(2*diagonal, 180, CV_8UC3);

    for (int x=0; x<edges.cols; x++)
    {
        for (int y=0; y<edges.rows; y++)
        {
            if (edges.at<uchar>(y, x) == 255)
            {
                for (int theta=0; theta<180; theta++)
                {
                    double rho = round( y*sin(theta*M_PI/180) + x*cos(theta*M_PI/180) ) + diagonal;
                    accumlate.at<cv::Vec3b>(rho, theta)[2] += 1;
                }
            }
        }
    }

    cv::resize(accumlate, accumlate, cv::Size(), 0.3*accumlate.rows/accumlate.cols, 0.3);

    std::vector<std::vector<int>> point(10, std::vector<int>(3, 0));

    // for (int i=0; i<10; i++)
    // {
    //     std::cout << point[i][0] << ", " << point[i][1] << std::endl;
    //     cv::circle(accumlate, cv::Point(point[i][1],point[i][0]), 20, cv::Scalar(0,255,0), 2);
    // }

    cv::namedWindow("Accumlation result", cv::WINDOW_NORMAL);
    cv::imshow("Accumlation result", accumlate);
    cv::imwrite("../result/hough_result.png", accumlate);

    cv::waitKey(0);
    return 0;
}
