#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

/**
 * 车牌识别
 * 1、车牌检测
 * 2、字符切割
 * 3、字符识别
*/

struct License
{
    cv::Mat mat;
    cv::Rect rect;
};

bool getLicenseROI(cv::Mat src, License &licenseROI)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresh;
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat open;
    cv::morphologyEx(thresh, open, cv::MORPH_OPEN, kernel);

    // 找到最外轮廓
    vector<vector<cv::Point>> contours;
    cv::findContours(open, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<cv::Point>> conPoly(contours.size());
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        double peri = cv::arcLength(contours[i], true);

        // 根据面积筛选出可能属于车牌的轮廓
        if (area > 1000)
        {
            // 使用多边形近似，进一步确定车牌区域轮廓
            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            if (conPoly[i].size() == 4)
            {
                cv::Rect box = cv::boundingRect(contours[i]);
                double ratio = double(box.width) / double(box.height);
                if (ratio > 2 && ratio < 4)
                {
                    cv::Rect rect = cv::boundingRect(contours[i]);
                    licenseROI = {src(rect), rect};
                }
            }
        }
    }

    if (licenseROI.mat.empty())
    {
        return false;
    }
    return true;
}

bool getCharacterROI(License &licenseROI, vector<License>& characterROI)
{
    cv::Mat gray;
    cv::cvtColor(licenseROI.mat, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresh;
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat close;
    cv::morphologyEx(thresh, close, cv::MORPH_CLOSE, kernel);

    vector<vector<cv::Point>> contours;
    cv::findContours(close, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        // 由于筛选出来的轮廓是无序的，后续需要将字符重新排序
        if (area > 200)
        {
            cv::Rect rect = cv::boundingRect(contours[i]);
            // 计算外接矩形宽高比
            double ratio = double(rect.height) / double(rect.width);
            if (ratio > 1)
            {
                cv::Mat roi = licenseROI.mat(rect);
                cv::resize(roi, roi, cv::Size(50, 100), 1, 1, cv::INTER_LINEAR);
                characterROI.push_back({roi, rect});
            }
        }
    }

    // 将筛选出来的字符轮廓，按照其左上角点坐标从左到右依次顺序排列
    // 冒泡排序
    for (int i = 0; i < characterROI.size() - 1; i++)
    {
        for (int j = 0; j < characterROI.size() - 1 - i; j++)
        {
            if (characterROI[j].rect.x > characterROI[j + 1].rect.x)
            {
                License temp = characterROI[j];
                characterROI[j] = characterROI[j + 1];
                characterROI[j + 1] = temp;
            }
        }
    }

    if (characterROI.size() != 7)
    {
        return false;
    }
    return true;
}

string UTF8ToGB(const char* str)
{
    string result;
    wchar_t *strSrc;
    // LPSTR
}