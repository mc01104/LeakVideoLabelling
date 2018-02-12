// LeakVideoLabelling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Utilities.h"

const double MAX_SPEED = 8.0;
const double MIN_SPEED = 0.1;

struct MouseCallbackData
{
	bool leakInFieldOfView;
	int	 centroid[2];
	::std::string  imageId;
	bool trackingMode;

	MouseCallbackData() : leakInFieldOfView(false)
	{
		imageId = "";
		centroid[0] = centroid[1] = -1;
		trackingMode = false;
	};

}data;

struct Params
{
	double speed;
	bool pause;
	bool quit;

	Params() : speed(1), pause(false), quit(false)
	{};

}params;

void labelVideosForLeakDetection(const ::std::string& path_to_video);
void processKeyboardInput(char key, Params& params);
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
void renderLeak(::cv::Mat& img, MouseCallbackData& data);
void saveLabels(MouseCallbackData& data, ::std::ostream& os);

int _tmain(int argc, _TCHAR* argv[])
{
	::std::string filename = "Z:/Public/Data/Cardioscopy_project/2018-02-06_bypass_cardioscopy/Videos_2018-02-06/2018-02-06_13-12-43";
	if (argc > 1)
	{
		::std::wstring test(argv[1]); 
		::std::string tmp(test.begin(), test.end()); 
		filename.assign(tmp);
	}
	labelVideosForLeakDetection(filename);

	return 0;
}


void labelVideosForLeakDetection(const ::std::string& path_to_video)
{
	::std::cout << "Labelling video: " << path_to_video << ::std::endl;

	::std::vector<::std::string> imList;
	int count = getImList(imList, checkPath(path_to_video + "/") );
	if (count > 0)
		std::sort(imList.begin(), imList.end(), numeric_string_compare);	

	::std::cout << "Number of frames:" << count << ::std::endl;
	
    //Create a window
	::cv::namedWindow("Display", 1);

	//set the callback function for any mouse event
	::cv::setMouseCallback("Display", CallBackFunc, &data);

	for (int i = 0; i < count; ++i)
	{
		char str[100];
		sprintf(str, "%2.1f", params.speed );

		::cv::Mat img = ::cv::imread(checkPath(path_to_video + "/" + imList[i]));
		data.imageId = imList[i];
		::cv::putText(img, ::cv::String(::std::string(str) +"x"), ::cv::Point(img.rows * 0.1, img.cols * (1 - 0.2)), CV_FONT_HERSHEY_PLAIN , 1.0, ::cv::Scalar(255, 255, 255));

		if (data.leakInFieldOfView)
			renderLeak(img, data);

		::cv::imshow("Display", img);
	
		saveLabels(data, ::std::cout);

		char key = ::cv::waitKey(10 * 1.0/params.speed * static_cast<int>(!params.pause));
		processKeyboardInput(key, params);


		if (params.quit)
			return;
	}
}

void processKeyboardInput(char key, Params& params)
{
	switch (key)
	{
		case 'p':
			params.pause = !params.pause;
			break;
		case 'q':
			params.quit = true;
			break;
		case 'f': 
			params.speed *= 2.0;
			params.speed = ::std::min(params.speed, MAX_SPEED);
			break;
		case 's': 
			params.speed *= 0.5;
			params.speed = ::std::max(params.speed, MIN_SPEED);
			break;
	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	 MouseCallbackData* data = reinterpret_cast<MouseCallbackData*> (userdata);
	 
     if  ( event == ::cv::EVENT_LBUTTONUP)
     {
		 data->leakInFieldOfView = ! data->leakInFieldOfView;
		 data->centroid[0] = -1;
		 data->centroid[1] = -1;
		 if (data->leakInFieldOfView)
		 {
			 data->centroid[0] = x; 
			 data->centroid[1] = y;
		 }
     }
	 else if (flags == (::cv::EVENT_MOUSEMOVE + ::cv::EVENT_FLAG_CTRLKEY))
	 {
		 if (data->leakInFieldOfView)
		 {
			 data->centroid[0] = x; 
			 data->centroid[1] = y;
		 }
	 }
}

void renderLeak(::cv::Mat& img, MouseCallbackData& data)
{
	double width = 50;
	double height = 50;

	::cv::Rect rec = ::cv::Rect(data.centroid[0] - 0.5 * width, data.centroid[1] - 0.5 * height, width, height);
	::cv::rectangle(img, rec, ::cv::Scalar(0, 0, 255), 2);
	::cv::circle(img, ::cv::Point(data.centroid[0], data.centroid[1]), 1, ::cv::Scalar(0, 0, 255), -1);
}

void saveLabels(MouseCallbackData& data, ::std::ostream& os)
{
	if (data.leakInFieldOfView)
		os << data.imageId << ", 1, " << data.centroid[0] << ", " << data.centroid[1] << ::std::endl;
	else
		os << data.imageId << ", 0, " << -1 << ", " << -1 << ::std::endl;
}