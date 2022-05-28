#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;

/**
 * 图像透视矫正
 * 轮廓提取
 *  提取最外轮廓
 *  提取矩形四个角点
 *  将矩形角点排序
 * 透视矫正
 **/

double EuDis(cv::Point pt1, cv::Point pt2)
{
    return sqrt((pt2.x - pt1.x) * (pt2.x - pt1.x) + (pt2.y - pt1.y) * (pt2.y - pt1.y));
}

int main()
{
    string src_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/perspective.jpg";
    string dst_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/perspective_result.jpg";
    
    cv::Mat src = cv::imread(src_file);
    if (src.empty())
    {
        cout << "No Image!" << endl;
        return -1;
    }

    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::Mat gaussian;
    cv::GaussianBlur(gray, gaussian, cv::Size(3, 3), 0); // 高斯模糊

    cv::Mat thresh;
    cv::threshold(gaussian, thresh, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat open;
    cv::morphologyEx(thresh, open, cv::MORPH_OPEN, kernel);

    cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::Mat close;
    cv::morphologyEx(open, close, cv::MORPH_CLOSE, kernel1);

    vector<vector<cv::Point>> contours;
    cv::findContours(close, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    vector<vector<cv::Point>> conPoly(contours.size());
    vector<cv::Point> srcPts;

    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);

        if (area > 10000)
        {
            double peri = cv::arcLength(contours[i], true);

            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            srcPts = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
        }
    }

    int width = src.cols / 2;
    int height = src.rows / 2;
    int T_L, T_R, B_R, B_L;

    for (int i = 0; i < srcPts.size(); i++)
    {
        if (srcPts[i].x < width && srcPts[i].y < height)
        {
            T_L = i;
        }
        if (srcPts[i].x > width && srcPts[i].y < height)
        {
            T_R = i;
        }
        if (srcPts[i].x < width && srcPts[i].y > height)
        {
            B_L = i;
        }
        if (srcPts[i].x > width && srcPts[i].y > height)
        {
            B_R = i;
        }
    }

    cv::circle(src, srcPts[T_L], 10, cv::Scalar(0, 0, 255), -1);
	cv::circle(src, srcPts[T_R], 10, cv::Scalar(0, 255, 255), -1);
	cv::circle(src, srcPts[B_R], 10, cv::Scalar(255, 0, 0), -1);
	cv::circle(src, srcPts[B_L], 10, cv::Scalar(0, 255, 0), -1);

    double leftHeight = EuDis(srcPts[T_L], srcPts[B_L]);
    double rightHeight = EuDis(srcPts[T_R], srcPts[B_R]);
    double maxHeight = max(leftHeight, rightHeight);

    double upWidth = EuDis(srcPts[T_L], srcPts[T_R]);
    double downWidth = EuDis(srcPts[B_L], srcPts[B_R]);
    double maxWidth = max(upWidth, downWidth);

    cv::Point2f srcAffinePts[4] = {cv::Point2f(srcPts[T_L]), cv::Point2f(srcPts[T_R]), cv::Point2f(srcPts[B_R]), cv::Point2f(srcPts[B_L])};
    cv::Point2f dstAffinePts[4] = {cv::Point2f(0, 0), cv::Point2f(maxWidth, 0), cv::Point2f(maxWidth, maxHeight), cv::Point2f(0, maxHeight)};

    cv::Mat M = cv::getPerspectiveTransform(srcAffinePts, dstAffinePts);

    cv::Mat dstImg;
    cv::warpPerspective(src, dstImg, M, cv::Point(maxWidth, maxHeight));

    cv::imwrite(dst_file, dstImg);

}