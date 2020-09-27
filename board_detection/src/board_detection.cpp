#include <cmath>
#include <eigen3/Eigen/Dense>
#include <opencv2/opencv.hpp>

#define deg2rad(deg) (deg*M_PI/180)

const int B_MAX = 70;
const int B_MIN = 0;
const int G_MAX = 200;
const int G_MIN = 100;
const int R_MAX = 210;
const int R_MIN = 100;

void calcIntersectionPoint(const int rho_1, const int theta_1, const int rho_2, const int theta_2)
{
    const double delta = sin(deg2rad(theta_1))*cos(deg2rad(theta_2)) - cos(deg2rad(theta_1))*sin(deg2rad(theta_2));
    Eigen::Matrix2d A;
    A <<  cos(deg2rad(theta_2)), sin(deg2rad(theta_2)),
          cos(deg2rad(theta_1)), sin(deg2rad(theta_1));
    Eigen::VectorXd B(2);
    B <<  -rho_1*cos(deg2rad(theta_1))+rho_2*cos(deg2rad(theta_2)),
          -rho_1*sin(deg2rad(theta_1))+rho_2*sin(deg2rad(theta_2));
    Eigen::MatrixXd C;
    C = A*B/delta;
    std::cout << "Here is the vector v:\n" << C << std::endl;
}

int main(int argc, char *argv[])
{
    cv::Mat image;
    cv::Mat mask, edges;
    cv::Mat result;


    // Load source image
    image = cv::imread("../../resources/plane.jpeg");
	if (image.empty())
    {
		std::cerr << "Not found input image..." << std::endl;
		return -1;
	}
    cv::resize(image, image, cv::Size(), 0.6, 0.6);
    result = image.clone();

	// inRangeを用いて色抽出フィルタリング
	cv::Scalar s_min = cv::Scalar(B_MIN, G_MIN, R_MIN);
	cv::Scalar s_max = cv::Scalar(B_MAX, G_MAX, R_MAX);
	cv::inRange(image, s_min, s_max, mask);

    // マスク画像のOpening処理
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::Mat());
    cv::threshold(mask, mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); // 2値化（閾値を自動で設定）

    // Canny edge
    // cv::blur(image, image, cv::Size(3, 3));
    cv::Canny(mask, edges, 200, 600, 3);

    cv::namedWindow("Canny result", cv::WINDOW_NORMAL);
    cv::imshow("Canny result", edges);

/*
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
                    double rho = round( y*sin(deg2rad(theta)) + x*cos(deg2rad(theta)) ) + diagonal;
                    accumlate.at<uchar>(rho, theta)+=2;
                    // accumlate.at<cv::Vec3b>(rho, theta)[2]++;
                }
            }
        }
    }
    cv::resize(accumlate, accumlate, cv::Size(), 0.3*accumlate.rows/accumlate.cols, 0.3);   // 確認用の縦横比調整


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
    for (int i=0; i<5; i++)
    {
        std::cout << "(vote, rho, theta) : " << votes[i][0] << "," << votes[i][1] << "," << votes[i][2] << std::endl;
        // cv::circle(accumlate, cv::Point(votes[i][2],votes[i][1]), 15, 255, 1);
    }
    cv::namedWindow("Accumlation result", cv::WINDOW_NORMAL);
    cv::imshow("Accumlation result", accumlate);
    cv::imwrite("../result/hough_result.png", accumlate);
*/


    std::vector<cv::Vec4i> lines;
    HoughLinesP( edges, lines, 1, CV_PI/180, 80, 30, 20 );
    cv::cvtColor(edges, edges, CV_GRAY2BGR);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::line( result, cv::Point(lines[i][0], lines[i][1]),
                    cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 2, 4 );
    }

    cv::namedWindow("result", cv::WINDOW_NORMAL);
    cv::imshow("result", result);

    cv::waitKey(0);
    return 0;
}
