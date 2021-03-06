// ================ development environment ￡oopencv 3.0

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <iostream>

#include "kcftracker.hpp"

#define VideoID 0

cv::Mat capframe;
const char* windowname = "tracker";
bool select_flag = false;
cv::Rect select;
cv::Point origin;
bool bRenewROI = false;  // the flag to enable the implementation of KCF algorithm for the new chosen ROI
bool bBeginKCF = false;

void onMouse(int event, int x, int y, int, void*)
{
	//Point origin;//2??ü?ú?a??μ?·???DD?¨ò?￡?òò?a?aê??ùóú???￠?ìó|μ?oˉêy￡??′DDíêoóorigin?íêí·?á?￡??ùò?′?2?μ?D§1??￡
	if (select_flag)
	{
		select.x = MIN(origin.x, x);		//2?ò??¨òaμèêó±êμˉ?e2???????D??ò￡???ó|???úêó±ê°′???aê?μ?μˉ?e?a??ê±??êμê±?????ù????D??ò
		select.y = MIN(origin.y, y);
		select.width = abs(x - origin.x);	//????D??í?èoí???è
		select.height = abs(y - origin.y);
		select &= cv::Rect(0, 0, capframe.cols, capframe.rows);//±￡?¤?ù????D??ò?úêó?μ??ê???óò???ú
	}
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		bBeginKCF = false;  // stop the KCF and display the ROI rectangle user chosed with the mouse
		select_flag = true;	// êó±ê°′??μ?±ê???3???μ
		origin = cv::Point(x, y);		//±￡′???à′μ￥?÷ê?2?×?μ?μ?μ?
		select = cv::Rect(x, y, 0, 0);	//?aà?ò??¨òa3?ê??ˉ￡??íoí???a(0,0)ê?òò?a?úopencv?DRect??D??òàà?úμ?μ?ê?°üo?×óé???????μ?μ?￡?μ?ê?2?o?óò????????μ?
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		select_flag = false;
		bRenewROI = true;
	}
}


int main(int argc, char* argv[])
{

	cv::VideoCapture  cap(VideoID);
	
	const char* windowname = "KCF_Tracking";
	bool stop = false;
	
	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool LAB = false;
	
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	if (!cap.isOpened()){
		std::cout << "Can not open camera." << std::endl;
	}
	// window for displaying the video stream 
	cv::namedWindow(windowname, 1);
	// capture for the mouse movement
	cv::setMouseCallback(windowname, onMouse, 0);

	cv::Rect KCFResult;
	
	while (!stop)
	{
		cap >> capframe;

		if (bRenewROI)
		{
			KCFResult = select;
			tracker.init(KCFResult, capframe);
			bBeginKCF = true;
			bRenewROI = false;
		}

		if (bBeginKCF)
		{
			KCFResult = tracker.update(capframe);
			std::cout << "width: " << KCFResult.width << "  height: " << KCFResult.height << std::endl;
			cv::rectangle(capframe, KCFResult, cv::Scalar(0, 0, 255), 2, 8, 0);
		}
		else
			cv::rectangle(capframe, select, cv::Scalar(255, 0, 0), 2, 8, 0);

		imshow(windowname, capframe);

		if (cv::waitKey(1) == 27)  // ESC 27  
			stop = true;

		std::cout << "x=" << select.x << " y=" << select.y << std::endl;

	}

	return 1;
}