#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

/**
 * 二维码检测
 * 
 * */

void Find_QR_Rect(cv::Mat src, vector<cv::Mat> &ROI_Rect)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(3, 3), 0);

    cv::Mat bin;
    cv::threshold(blur, bin, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));
    cv::Mat open;
    cv::morphologyEx(bin, open, cv::MORPH_OPEN, kernel);

    // 通过Size(21, 1)闭运算能够有效地将矩形区域连接，便于提取矩形区域
    cv::Mat kernel1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 1));
    cv::Mat close;
    cv::morphologyEx(open, close, cv::MORPH_CLOSE, kernel1);

    // 使用RETR_EXTERNAL找到最外轮廓
    vector<vector<cv::Point>> MaxContours;
    cv::findContours(close, MaxContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < MaxContours.size(); i++)
    {
        cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC3);
        mask = cv::Scalar::all(255);

        double area = cv::contourArea(MaxContours[i]);

        // 通过面积阈值找到二维码所在矩形区域
        if (area > 6000 && area < 100000)
        {
            // 计算最小外接矩形
            cv::RotatedRect MaxRect = cv::minAreaRect(MaxContours[i]);
            // 计算最小外接矩形宽高比
            double ratio = MaxRect.size.width / MaxRect.size.height;

            if (ratio > 0.8 && ratio < 1.2)
            {
                cv::Rect MaxBox = MaxRect.boundingRect();

                // 将矩形区域从原图中抠出来
                cv::Mat ROI = src(cv::Rect(MaxBox.tl(), MaxBox.br()));

                ROI.copyTo(mask(MaxBox));

                ROI_Rect.push_back(mask);
            }
        }
    }
}

int Detect_QR_Rect(cv::Mat src, cv::Mat &canvas, vector<cv::Mat>& ROI_Rect)
{
    vector<vector<cv::Point>> QR_Rect;

    // 遍历所有找到的矩形区域
    for (int i = 0; i < ROI_Rect.size(); i++)
    {
        cv::Mat gray;
        cv::cvtColor(ROI_Rect[i], gray, cv::COLOR_BGR2GRAY);

        cv::Mat bin;
        cv::threshold(gray, bin, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

        // 通过hierarchy, RETR_TREE找到轮廓之间的层级关系
        vector<vector<cv::Point>> contours;
        vector<cv::Vec4i> hierarchy;
        cv::findContours(bin, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        // 父轮毂索引
        int ParentIndex = -1;
        int cn = 0;

        // 用于存储二维码矩形的三个“回”
        vector<cv::Point> rect_points;
        for (int i = 0; i < contours.size(); i++)
        {
            if (hierarchy[i][2] != -1 && cn == 0)
            {
                ParentIndex = i;
                cn++;
            }
            else if (hierarchy[i][2] != -1 && cn == 1)
            {
                cn++;
            }
            else if (hierarchy[i][2] == -1)
            {
                ParentIndex = -1;
                cn = 0;
            }

            // 如果该轮廓存在子轮廓，且有2级子轮廓则认定找到“回”
            if (hierarchy[i][2] != -1 && cn == 2)
            {
                cv::drawContours(canvas, contours, ParentIndex, cv::Scalar::all(255), -1);

                cv::RotatedRect rect;

                rect = cv::minAreaRect(contours[ParentIndex]);

                rect_points.push_back(rect.center);
            }
        }

        // 将找到地“回”连接起来
        for (int i = 0; i < rect_points.size(); i++)
        {
            cv::line(canvas, rect_points[i], rect_points[(i+1) % rect_points.size()], cv::Scalar::all(255), 5);
        }

        QR_Rect.push_back(rect_points);
    }

    return QR_Rect.size();
}

int main()
{
    string src_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/qr_code.jpg";
    string dst_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/qr_code_result.jpg";
    cv::Mat src = cv::imread(src_file);

    if (src.empty())
    {
        cout << "No image data!" << endl;
        return 0;
    }

    vector<cv::Mat> ROI_Rect;
    Find_QR_Rect(src, ROI_Rect);

    cout << "Find_QR_Rect " << ROI_Rect.size() << endl;

    cv::Mat canvas = cv::Mat::zeros(src.size(), src.type());

    int flag = Detect_QR_Rect(src, canvas, ROI_Rect);

    if (flag <= 0)
    {
        cout << "Can not detect QR code!" << endl;
        return 0;
    }

    cout << "检测到" << flag << "个二维码" << endl;

    // 画出二维码位置
    cv::Mat gray;
    cv::cvtColor(canvas, gray, cv::COLOR_BGR2GRAY);

    vector<vector<cv::Point>> contours;
    cv::findContours(gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Point2f points[4];

    for (int i = 0; i < contours.size(); i++)
    {
        cv::RotatedRect rect = cv::minAreaRect(contours[i]);

        rect.points(points);

        for (int j = 0; j < 4; j++)
        {
            cv::line(src, points[j], points[(j+1)%4], cv::Scalar(0, 255, 0), 2);
        }
    }

    cv::imwrite(dst_file, src);

    return 0;
}