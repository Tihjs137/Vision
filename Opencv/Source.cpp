#include "opencv2/opencv.hpp"			// include the opencv library, needed in every project. Some other functions require other libraries
#include <opencv2/core/core.hpp>		// it could be that for this project you don't need all of the included libraries but you might in the future
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#pragma comment(lib, "ComCtl32.lib")

using namespace std;					// otherwise we need to use "std::" infront of all the std tasks
using namespace cv;						// otherwise we need to use "cv::" infront of all the opencv tasks
Mat Canny_picture;
Mat Output;								//for finising step :diplaying ball on original image 
Mat dst;								//For blurring
char zBuffer[35];						//      "
SimpleBlobDetector::Params params;
SimpleBlobDetector detector;
SimpleBlobDetector::Params Setup_blob();
Mat blob(SimpleBlobDetector::Params params, Mat im);
bool vision();
const string trackbarWindowName = "Trackbars";
int H_MIN = 100;
int H_MAX = 140;
int S_MIN = 100;
int S_MAX = 256;
int V_MIN = 70;
int V_MAX = 256;
void on_trackbar(int, void*)
{}
void createTrackbars();


int main(int argc, char *argv[])
{
	vision();
}






bool vision()					// create the main loop
{
	VideoCapture cap(0);				// open the default camera, should you have a second external camera plugged in you can use "cap(1)" to take the camera
	if (!cap.isOpened())				// check if we succeeded
		return -1;						// if not we close the program

	params = Setup_blob();
	createTrackbars();					//Guess what.... this creates trackbars :o


	Mat frame;							// create the matrix in which the information frame of the camera will be safed
										// create the windows in which to show your result (multiple options like size)
	for (;;)							// create an endless for loop to keep the program running on this piece of code 
	{
		cap >> frame;					// get a new frame from camera
		Mat orig_image = frame.clone();		// Clone the input "frame" into the new matrix we made, this way you can use different functions on different images
		medianBlur(frame, frame, 3);		// blurring the input "frame" this way the filtering of the colors comes out better

		Mat hsv_image;						// create a new matrix for the HSV image
		cvtColor(frame, hsv_image, COLOR_BGR2HSV);	// Convert input image to HSV

		Mat ColorOnly;						// Create a new matrix
											// Threshold the HSV image, keep only the color pixels you want. Maybe google what HSV means to see how to use it
		//inRange(hsv_image, Scalar(100, 100, 70), Scalar(140, 255, 255), ColorOnly); // blue preset
		inRange(hsv_image, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), ColorOnly);

		namedWindow("ColorFilter", WINDOW_AUTOSIZE);
		imshow("ColorFilter", ColorOnly);			// show the HSV filter output in the window called "ColorFilter"
		namedWindow("Input", WINDOW_AUTOSIZE);
		imshow("Input", orig_image);		// show the camera picture in the window called "Input"
		
		GaussianBlur(ColorOnly, dst, Size(35, 35), 0, 0);

		//cout << "Before blob \n";
		Mat blobs = blob(params, ColorOnly);
		if (!blobs.empty())
		{
			imshow("Blob", blobs);
		}
		
		
		//show the blurred image with the text
		imshow("Smoothed Image", dst);
				
		Canny(dst, Canny_picture, 40, 60, 3);
		namedWindow("Canny edge", WINDOW_AUTOSIZE);
		imshow("Canny edge", Canny_picture);


		Output = orig_image.clone();
		Output.setTo(255, Canny_picture);
		imshow("output", Output);

		if (waitKey(30) >= 0) break;		// if you press any key while in the output window the program will close
	}
	return 0; 								// the camera will be deinitialized automatically in VideoCapture destructor
}

Mat blob(SimpleBlobDetector::Params params, Mat im)
{
	Mat dst;
	bitwise_not(im, dst);
	// Detect blobs.
	std::vector<KeyPoint> keypoints;

	Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
	detector->detect(dst, keypoints);

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat im_with_keypoints;
	drawKeypoints(dst, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	
	//This loop fetches the locations from the keypoint data container
	if ( keypoints.size() != 0)			//Signed/unsigned variable comparison :o			
	{
		for (int i = 0; i < keypoints.size(); i++)
		{
			float x = keypoints[i].pt.x;
			float y = keypoints[i].pt.y;
			float width = keypoints[i].size;

			cout << "Found object number: " << i <<" at  x: " << x << ", y: " << y << " Size: "<< width <<" pixels"<< "\n";  //here it is printed to the terminal
		}
	}

	return im_with_keypoints;
}

SimpleBlobDetector::Params Setup_blob()
{
	// Change thresholds
	params.minThreshold = 100;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1500;

	// Filter by Circularity
	params.filterByCircularity = false;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = false;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = false;
	params.minInertiaRatio = 0.01;
	return params;
}

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}