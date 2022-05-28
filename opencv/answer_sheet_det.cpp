#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

/**
 * 答题卡识别
 * 1、图像透视矫正，将答题卡区域正确切割出来
 * 2、将每一题号分别抠出来存为新图像，待识别
 * 3、对每一题号确定其A,B,C,D,E选项区域，统计其像素点个数，故而匹配选项 
 **/

void GetWarpImg(cv::Mat src, cv::Mat& warpImg)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(3, 3), 0);

    cv::Mat canny;
    cv::Canny(blur, canny, 100, 200);

    vector<vector<cv::Point>> contours;

    cv::findContours(canny, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int index = 0;
    double maxArea = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);

        if (maxArea < area)
        {
            maxArea = area;
            index = i;
        }
    }

    // 多边形近似
    vector<vector<cv::Point>> conPoly(contours.size());

    double peri = cv::arcLength(contours[index], true);

    cv::approxPolyDP(contours[index], conPoly[index], 0.02 * peri, true);

    // 找到矩形的四个顶点
    vector<cv::Point> srcPts;
    for (int i = 0; i < conPoly[index].size(); i++)
    {
        srcPts.push_back(cv::Point(conPoly[index][i].x, conPoly[index][i].y));
    }

    int width = src.cols / 2;
    int height = src.rows / 2;

    // 将矩形四个顶点按T_L, B_L, B_R, T_R区分
    int T_L, B_L, B_R, T_R;
    for (int i = 0; i < srcPts.size(); i++)
    {
        if (srcPts[i].x < width && srcPts[i].y < height)
        {
            T_L = i;
        }
        if (srcPts[i].x < width && srcPts[i].y > height)
        {
            B_L = i;
        }
        if (srcPts[i].x > width && srcPts[i].y > height)
        {
            B_R = i;
        }
        if (srcPts[i].x > width && srcPts[i].y < height)
        {
            T_R = i;
        }
    }

    /**
     * 变换后，图像长宽
     * 长 = max(变换前左边长，变换前右边长)
     * 宽 = max(变换前上边长，变换前下边长)
     * 设变换后图像的左上角位置为原点
    **/
    // double upWidth = EuDis()
}