#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	// When the user clicks on the screen, this function will take a picture an change it to gray scale.

	if (event == EVENT_LBUTTONDOWN) {
		//cout << "Left button of the mouse has been clicked." << x << " " << y << endl;
		namedWindow("Result", WINDOW_AUTOSIZE);

		UMat* frame = (UMat*)userdata;
		UMat gray;
		cvtColor(*frame, gray, COLOR_BGR2GRAY);
		imshow("Result", gray);
	}
	
}

int main()
{
	namedWindow("start", WINDOW_AUTOSIZE);

	// This is where the video capture will go.
	UMat bgr_frame;

	VideoCapture cap;
	cap.open(0);

	// Make sure the video capture initialized.
	if (!cap.isOpened())
	{
		cerr << "Couldn't open capture." << endl;
	}
	
	// Start the video capture, until esc
	for (;;)
	{
		cap >> bgr_frame;
		if (bgr_frame.empty()) break;

		// Display video capture
		imshow("start", bgr_frame);

		// When the mouse is clicked, take a picture
		setMouseCallback("start", CallBackFunc, &bgr_frame);

		// Exit when esc is pressed.
		char c = waitKey(10);
		if (c == 27) break;
		
	}
	
	// Release video capture
	cap.release();
	

	return 0;
}