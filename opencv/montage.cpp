#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;

/**
 * 合成蒙太奇图像
 **/

const int step_x = 20;
const int step_y = 20;

int getImagePathList(string folder, vector<string> &imagePathList)
{
    cv::glob(folder, imagePathList);
    return 0;
}

int main()
{
    string src_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/taylor.jpg";
    string dst_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/taylor_dst.jpg";
    string montage_dst_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/montage_taylor.jpg";
    string folder = "images/";

    cv::Mat src = cv::imread(src_file);
    if (src.empty())
    {
        cout << "No image!" << endl;
        return 0;
    }

    cv::resize(src, src, cv::Size(step_x * 30, step_y * 30), 1, 1, cv::INTER_CUBIC);

    vector<cv::Mat> images;
    

    cout << "loading..." << endl;
    vector<string> imagePathList;
    for (int i=0;i<30;i++)
    {
        imagePathList.push_back("/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/taylor_bg.jpg");
    }
    // getImagePathList(folder, imagePathList);

    for (int i = 0; i < imagePathList.size(); i++)
    {
        cv::Mat img = cv::imread(imagePathList[i]);
        cv::resize(img, img, cv::Size(step_x, step_y), 1, 1, cv::INTER_CUBIC);
        images.push_back(img);
    }

    cout << "done!" << endl;

    int rows = src.rows;
    int cols = src.cols;

    int height = rows / step_y, width = cols / step_x;  

    cv::Mat temp;
    cv::Mat dst = cv::Mat(src.size(), CV_8UC3, cv::Scalar(255, 255, 255));

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int index = i * width + j;
            images[index].copyTo(temp);
            cout << "debug " << index << endl;
            temp = dst(cv::Rect(j * step_x, i * step_y, step_x, step_y));
        }
    }
    
    cv::imwrite(dst_file, dst);

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            dst.at<cv::Vec3d>(i, j)[0] = 0.312 * dst.at<cv::Vec3d>(i, j)[0] + 0.698 * src.at<cv::Vec3d>(i, j)[0];
            dst.at<cv::Vec3d>(i, j)[1] = 0.312 * dst.at<cv::Vec3d>(i, j)[1] + 0.698 * src.at<cv::Vec3d>(i, j)[1];
            dst.at<cv::Vec3d>(i, j)[2] = 0.312 * dst.at<cv::Vec3d>(i, j)[2] + 0.698 * src.at<cv::Vec3d>(i, j)[2];
        }
    }

    cv::imwrite(montage_dst_file, dst);
    return 0;
}