# DesktopGoggles

## Overview
DesktopGoggles is a desktop application that let's users google text on items with their webcams. I wanted to do this project to review my C++ basics and learn more advanced C++. Moreover, I wanted to learn how to use OpenCV and Tesseract OCR.

## What I've learned
As expected, I was able to review the basic of C++ and gain a little experience using the newer features of C++ such as vectors. I also learned how to install C++ libraries using Microsoft Visual Studio 2017 using CPPAN and Cmake. The code that I use for text detection and text recognition is available from the sample projects that make sure you have everything up and running. It is available [here](https://github.com/opencv/opencv_contrib/blob/master/modules/text/samples/end_to_end_recognition.cpp). I realized pretty late that the code used to check if my development environment was working can be used for this project. I am not sure how to properly attribute the code to the original author. I've also learned more about computer vision and the limitations of current OCR technology. I also learned how to open URLs in the default browser using C++ in Windows.

## Improvements
There are a lot of improvements that can be done to this project. First, to improve the recognition/detection of text, the parameters of some of the necessary functions should be played with. I think this should reasonably improve the functioning of the app despite the limitations of Tesseract. Second, I should create a GUI around the application to make it easier for others to use.

## How to run this on your computer
1. Install CPPAN, CMAKE and Git
2. Fork this repo using your command line
3. Then, change the current working directory to the folder that contains the repo.
4. Then run 'cppan --build main.cpp'
5. A visual studio solution file should be created if Microsoft C++ is your default compiler. Check online for other compilers
6. Build and run solution in Microsoft visual studio.
