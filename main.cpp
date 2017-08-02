/*
dependencies:
    - pvt.cppan.demo.intel.opencv.core: 3
    - pvt.cppan.demo.intel.opencv.contrib.text: 3
    - pvt.cppan.demo.intel.opencv.highgui: 3
    - pvt.cppan.demo.intel.opencv.features2d: 3
    - pvt.cppan.demo.intel.opencv.videoio: 3
	- pvt.cppan.demo.qtproject.qt.base.gui: 5.9.1
*/

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/text.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>



#include <iostream>
#include <numeric>

using namespace cv;
using namespace cv::text;
using namespace std;

void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation);
bool isRepetitive(const string& s);

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	// When the user clicks on the screen, this function will take a picture an change it to gray scale.

	if (event == EVENT_LBUTTONDOWN) {
		namedWindow("Result", WINDOW_AUTOSIZE);

		UMat* frame = (UMat*)userdata;

		// Channels that are going to be processed individually
		vector<Mat> channels;

		// Use a grey channel
		Mat grey;
		cvtColor(*frame, grey, COLOR_RGB2GRAY);

		channels.push_back(grey);
		channels.push_back(255 - grey);

		// Create ERFilter with default classifiers
		Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("./trained_classifierNM1.xml"),8,0.00015f,0.13f,0.2f,true,0.1f);
		Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("./trained_classifierNM2.xml"), 0.5);

		vector<vector<ERStat>> regions(channels.size());

		for (int c = 0; c<(int)channels.size(); c++)
		{
			er_filter1->run(channels[c], regions[c]);
			er_filter2->run(channels[c], regions[c]);
		}

		Mat out_img_decomposition = Mat::zeros(frame->rows + 2, frame->cols + 2, CV_8UC1);
		vector<Vec2i> tmp_group;
		for (int i = 0; i<(int)regions.size(); i++)
		{
			for (int j = 0; j<(int)regions[i].size(); j++)
			{
				tmp_group.push_back(Vec2i(i, j));
			}
			Mat tmp = Mat::zeros(frame->rows + 2, frame->cols + 2, CV_8UC1);
			er_draw(channels, regions, tmp_group, tmp);
			if (i > 0)
				tmp = tmp / 2;
			out_img_decomposition = out_img_decomposition | tmp;
			tmp_group.clear();
		}

		vector< vector<Vec2i> > nm_region_groups;
		vector<Rect> nm_boxes;
		erGrouping(*frame, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_HORIZ);

		Ptr<OCRTesseract> ocr = OCRTesseract::create();

		string output;

		Mat out_img;
		Mat out_img_detection;
		Mat out_img_segmentation = Mat::zeros(frame->rows + 2, frame->cols + 2, CV_8UC1);
		frame->copyTo(out_img);
		frame->copyTo(out_img_detection);
		float scale_img = 600.f / frame->rows;
		float scale_font = (float)(2 - scale_img) / 1.4f;
		vector<string> words_detection;

		for (int i = 0; i<(int)nm_boxes.size(); i++)
		{

			rectangle(out_img_detection, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(0, 255, 255), 3);

			Mat group_img = Mat::zeros(frame->rows + 2, frame->cols + 2, CV_8UC1);
			er_draw(channels, regions, nm_region_groups[i], group_img);
			Mat group_segmentation;
			group_img.copyTo(group_segmentation);
			group_img(nm_boxes[i]).copyTo(group_img);
			copyMakeBorder(group_img, group_img, 15, 15, 15, 15, BORDER_CONSTANT, Scalar(0));

			vector<Rect>   boxes;
			vector<string> words;
			vector<float>  confidences;
			ocr->run(group_img, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);

			output.erase(remove(output.begin(), output.end(), '\n'), output.end());

			if (output.size() < 3)
				continue;

			for (int j = 0; j<(int)boxes.size(); j++)
			{
				boxes[j].x += nm_boxes[i].x - 15;
				boxes[j].y += nm_boxes[i].y - 15;

				if ((words[j].size() < 2) || (confidences[j] < 51) ||
					((words[j].size() == 2) && (words[j][0] == words[j][1])) ||
					((words[j].size()< 4) && (confidences[j] < 60)) ||
					isRepetitive(words[j]))
					continue;
				words_detection.push_back(words[j]);
				rectangle(out_img, boxes[j].tl(), boxes[j].br(), Scalar(255, 0, 255), 3);
				Size word_size = getTextSize(words[j], FONT_HERSHEY_SIMPLEX, (double)scale_font, (int)(3 * scale_font), NULL);
				rectangle(out_img, boxes[j].tl() - Point(3, word_size.height + 3), boxes[j].tl() + Point(word_size.width, 0), Scalar(255, 0, 255), -1);
				putText(out_img, words[j], boxes[j].tl() - Point(1, 1), FONT_HERSHEY_SIMPLEX, scale_font, Scalar(255, 255, 255), (int)(3 * scale_font));
				out_img_segmentation = out_img_segmentation | group_segmentation;
			}
			
			string toGoogle;
			toGoogle = accumulate(begin(words), end(words), toGoogle);
			cout << toGoogle << endl;

		}

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

void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation)
{
	for (int r = 0; r<(int)group.size(); r++)
	{
		ERStat er = regions[group[r][0]][group[r][1]];
		if (er.parent != NULL) // deprecate the root region
		{
			int newMaskVal = 255;
			int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
			floodFill(channels[group[r][0]], segmentation, Point(er.pixel%channels[group[r][0]].cols, er.pixel / channels[group[r][0]].cols),
				Scalar(255), 0, Scalar(er.level), Scalar(0), flags);
		}
	}
}

bool isRepetitive(const string& s)
{
	int count = 0;
	for (int i = 0; i<(int)s.size(); i++)
	{
		if ((s[i] == 'i') ||
			(s[i] == 'l') ||
			(s[i] == 'I'))
			count++;
	}
	if (count >((int)s.size() + 1) / 2)
	{
		return true;
	}
	return false;
}