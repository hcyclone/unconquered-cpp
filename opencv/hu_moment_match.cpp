#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

vector<vector<cv::Point>> findContour(cv::Mat image)
{
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresh;
    cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);

    vector<vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    vector<vector<cv::Point>> effectConts;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);

        if (area > 1000)
        {
            effectConts.push_back(contours[i]);
        }
    }
    return effectConts;
}

int main()
{
    string src_file = "../imgs/hand.jpg";
    string test_file = "../imgs/hand_test.jpg";
    string dst_file = "../imgs/hand_result.jpg";

    cv::Mat src = cv::imread(src_file);
    cv::Mat test = cv::imread(test_file);

    if (src.empty() || test.empty())
    {
        cout << "No image!" << endl;
        return 0;
    }

    vector<vector<cv::Point>> src_contours;
    vector<vector<cv::Point>> test_contours;

    src_contours = findContour(src);
    test_contours = findContour(test);

    cv::Moments m_test = cv::moments(test_contours[0]);
    cv::Mat hu_test;
    cv::HuMoments(m_test, hu_test);

    double minDis = 1000;
    int minIndex = 0;
    for (int i = 0; i < src_contours.size(); i++)
    {
        cv::Moments m_src = cv::moments(src_contours[i]);
        cv::Mat hu_src;
        cv::HuMoments(m_src, hu_src);

        double dist = cv::matchShapes(hu_test, hu_src, cv::CONTOURS_MATCH_I1, 0);

        if (dist < minDis)
        {
            minDis = dist;
            minIndex = i;
        }
    }

    cv::drawContours(src, src_contours, minIndex, cv::Scalar(0, 255, 0), 2);

    cv::Rect rect = cv::boundingRect(src_contours[minIndex]);

    cv::rectangle(src, rect, cv::Scalar(0, 0, 255), 2);

    cv::imwrite(dst_file, src);

    return 0;
}