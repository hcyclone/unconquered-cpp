#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
// using namespace cv;

void GetROI(cv::Mat src, cv::Mat& ROI)
{
    int width = src.cols;
    int height = src.rows;

    vector<cv::Point> pts;
    cv::Point ptA((width / 8) * 2, (height / 20) * 19);
    cv::Point ptB((width / 8) * 2, (height / 8) * 7);
    cv::Point ptC((width / 10) * 4, (height / 5) * 3);
    cv::Point ptD((width / 10) * 5, (height / 5) * 3);
    cv::Point ptE((width / 8) * 7, (height / 8) * 7);
    cv::Point ptF((width / 8) * 7, (height / 20) * 19);
    pts = {ptA, ptB, ptC, ptD, ptE, ptF};

    vector<vector<cv::Point>>ppts;
    ppts.push_back(pts);

    cv::Mat mask = cv::Mat::zeros(src.size(), src.type());
    cv::fillPoly(mask, ppts, cv::Scalar::all(255));

    src.copyTo(ROI, mask);
}

void DeleteRoadLine(cv::Mat src, cv::Mat &ROI)
{
    cv::Mat gray;
    cv::cvtColor(ROI, gray, cv::COLOR_BGR2GRAY);

    cv::Mat thresh;
    cv::threshold(gray, thresh, 180, 255, cv::THRESH_BINARY);

    vector<cv::Point> left_line;
    vector<cv::Point> right_line;

    for (int i = 0; i < thresh.cols / 2; i++)
    {
        for (int j = 0; j < thresh.rows; j++)
        {
            if (thresh.at<uchar>(j, i) == 255)
            {
                left_line.push_back(cv::Point(i, j));
            }
        }
    }

    for (int i = thresh.cols / 2; i < thresh.cols; i++)
    {
        for (int j = 0; j < thresh.rows; j++)
        {
            if (thresh.at<uchar>(j, i) == 255)
            {
                right_line.push_back(cv::Point(i, j));
            }
        }
    }

    if (left_line.size() > 0 && right_line.size() > 0)
    {
        cv::Point B_L = (left_line[0]);
        cv::Point T_L = (left_line[left_line.size() - 1]);
        cv::Point T_R = (right_line[0]);
        cv::Point B_R = (right_line[right_line.size() - 1]);

        cv::circle(src, B_L, 10, cv::Scalar(0, 0, 255), -1);
        cv::circle(src, T_L, 10, cv::Scalar(0, 255, 0), -1);
        cv::circle(src, T_R, 10, cv::Scalar(255, 0, 0), -1);
        cv::circle(src, B_R, 10, cv::Scalar(0, 255, 255), -1);

        cv::line(src, cv::Point(B_L), cv::Point(T_L), cv::Scalar(0, 255, 0), 10);
        cv::line(src, cv::Point(T_R), cv::Point(B_R), cv::Scalar(0, 255, 0), 10);

        vector<cv::Point> pts;
        pts = {B_L, T_L, T_R, B_R};
        vector<vector<cv::Point>> ppts;
        ppts.push_back(pts);

        cv::fillPoly(src, ppts, cv::Scalar(133, 230, 238));
    }
}

int main()
{
    cv::Mat frame = cv::imread("/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/lane_det.jpg");
    
    cv::Mat image;

    GetROI(frame, image);

    DeleteRoadLine(frame, image);

    cv::imwrite("/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/lane_det_result.jpg", frame);

    return 0;
}