#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

/**
 * 绿幕背景替换
*/


cv::Mat ReplaceBackground(cv::Mat frame, cv::Mat mask, cv::Mat bg)
{
    cv::Mat dst = cv::Mat::zeros(frame.size(), frame.type());

	for (int i = 0; i < frame.rows; i++)
	{
		for (int j = 0; j < frame.cols; j++)
		{
			int p = mask.at<uchar>(i, j);  //传入的mask是张二值图,p为当前mask像素值

			if (p == 0)
			{   //代表mask此时为前景，将绿幕视频中的前景像素赋给dst
				dst.at<cv::Vec3b>(i, j) = frame.at<cv::Vec3b>(i, j);
			}
			else if (p == 255)
			{
				//代表mask此时为背景，将背景图像素赋给dst
				dst.at<cv::Vec3b>(i, j) = bg.at<cv::Vec3b>(i,j);
			}

		}
	}

	return dst;
}

int main()
{
    string src_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/green_bg1.jpg"; // 绿幕
    string bg_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/green_bg.jpg"; // 背景
    string dst_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/green_bg2.jpg";
    string mask_file = "/mnt/cfs/algorithm/hangcong.li/code/cpp/unconquered-cpp/opencv/imgs/green_bg2_mask.jpg";

    cv::Mat frame = cv::imread(src_file);
    cv::Mat bg = cv::imread(bg_file);

    int width = frame.cols;
    int height = frame.rows;

    cv::resize(bg, bg, cv::Size(width, height), 1, 1, cv::INTER_CUBIC);

    cv::Mat HSV, mask, dst;

    int hmin = 35, smin = 43, vmin = 46;
    int hmax = 77, smax = 255, vmax = 255;


    cv::cvtColor(frame, HSV, cv::COLOR_BGR2HSV); // 将原图转换成HSV颜色空间

    // 经过inRange, 输入一张二值图像，将前景从绿幕中抠出来
    cv::inRange(HSV, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax, smax, vmax), mask);

    cv::imwrite(mask_file, mask);

    dst = ReplaceBackground(frame, mask, bg);
    cout << "1" << endl;

    cout << dst.size() << endl;

    cv::imwrite(dst_file, dst);

    return 0;
}