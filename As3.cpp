// Name        : As3.cpp
// Author      : Pan
// Version     : 13 MArch 2017
// Title   	   : Basic Frame Reading
// Description : This is a basic OpenCV program for reading and displaying
// 			live frames (images) from a camera.

#include <cv.h>
#include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;
using namespace cv;

int main(  int argc, char** argv ) {

	srand (time(NULL));
	int frameCount;
	int xROI = 175, yROI = 300; // top-left corner coordinates of ROI
	int wROI = 75, hROI = 75;	// dimensions of ROI
	int x, y, sMass, xMass, xCOM, yCOM = yROI + hROI/2; //shooter
	int life = 5, shield = 25; // radius of inner and outer shooter
	int xBullet, yBullet; //bullets
	int i, dX = 3, dY = 1 + rand()%4; // enemies
	int xBE, yBE, rBE = 5, dxBE = 1, dyBE = 3; //Enemy's bullets
	bool available = true;
	Mat frame;
	Mat refFrame;
	Mat diffRef;
	Mat prevFrame;
	Mat diffPrev;
	Mat game = Mat (480, 640, CV_8UC3);

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) {  // check if we succeeded
		return -1;
	}
	cap >> frame;
	frame.copyTo (refFrame);
	int width = frame.cols, height = frame.rows;
	cout <<"Frame dimension: "<< width <<"x"<< height <<endl;

	int xE[5], yE[5], b[5], g[5], r[5], radius[5];
	for (i = 0; i < 5; i++) {
		xE[i] = 50 + rand()%(width - 100);
		b[i] = 12 + rand()%230;
		g[i] =  128 + rand()%128;
		r[i] = 50 + rand()%205;
		radius[i] = 5 + rand()% 46;
		yE[i] = radius[i] + rand()%100;
	}
	i = 0;

	namedWindow ("Raw Image",CV_WINDOW_AUTOSIZE);
	//namedWindow ("Difference fr Reference", CV_WINDOW_AUTOSIZE);
	//namedWindow ("Difference fr Previous", CV_WINDOW_NORMAL);
	namedWindow ("Game", CV_WINDOW_AUTOSIZE);

	for(frameCount = 0; frameCount < 100000000; frameCount++) {
		rectangle (game, Rect(0,0, width, height), Scalar (0,0,0), -1);
		// to erase the whole frame before next frame is shown on Game window

		frame.copyTo (prevFrame);
		if (frameCount % 100 == 0) {
			printf("frameCount = %d \n", frameCount);
		}

		cap >> frame; // get a new frame from camera
		absdiff (refFrame, frame, diffRef);
		cvtColor (diffRef, diffRef, CV_BGR2GRAY, 1);
		absdiff (prevFrame, frame, diffPrev);
		cvtColor (diffPrev, diffPrev, CV_BGR2GRAY, 1);

	// Moving Shooter
		sMass = 0, xMass = 0;
			for (y = yROI; y < yROI + hROI; y++) {
				for (x = xROI; x < xROI + wROI; x++) {
					sMass += diffPrev.at<unsigned char>(y,x);
					xMass += diffPrev.at<unsigned char>(y,x)*x;
				}
			}

		xCOM = xMass/sMass;
		xROI = xCOM - wROI/2;
		if (xROI < 0) {
			xROI = 0;
		}
		if (xROI + wROI > frame.cols) {
			xROI = frame.cols - wROI;
		}
		if (yROI < 0) {
			yROI = 0;
		}
		if (yROI + hROI > frame.rows) {
			yROI = frame.rows - hROI;
		}

	//Bullets
		if (frameCount % 80 == 0) {
			xBullet = xCOM;
			yBullet = yCOM;
		}
		circle (game, Point (xBullet, yBullet), 5, Scalar (255,255,255), -1);
		yBullet += -9;

	// Enemies
		circle (game, Point (xE[i], yE[i]), radius[i],
								Scalar (b[i], g[i], r[i]), 3);
	//Levels ... [1..10]
// Instruction
		if (i == 0) {
			putText (game, "Level 1", Point(10, height - 10),
				FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			putText (game, "Shoot Target!", Point(200, height - 80),
				FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2);
		}
// Move on x-axis and shoot straight downwards
		if (i == 1) {
			putText (game, "Level 2", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			xE[i] += dX;
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (frameCount % 120 == 0) {
				xBE = xE[i];
				yBE = yE[i];
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (available == false && distE < rBE + life) {
				cout <<"Game over..."<<endl;
				break;
			}
			yBE += 4;
		}

//Bounce against walls and shoot in random direction
		if (i == 2) {
			putText (game, "Level 3", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			xE[i] += dX;
			yE[i] += -dY;
			if (xE[i] - radius[i] < 0) {
				xE[i] = radius[i];
				dX = -dX;
			}
			 if (xE[i] + radius[i] > width) {
				xE[i] = width - radius[i];
				dX = -dX;
			 }
			if (yE[i] - radius[i] < 0) {
				yE[i] = radius[i];
				dY = -dY;
			}
			if (yE[i] + radius[i] > height) {
				yE[i] = height - radius[i];
				dY = -dY;
			}
			if (xBE < 0 || xBE > width || yBE < 0 || yBE > height) {
				xBE = xE[i];
				yBE = yE[i];
				dxBE = (dxBE + 1) % 11 - 5;
				do {
					dyBE = -5 + rand()%11;
				} while (dyBE > -2 && dyBE < 3);
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);

			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (available == false && distE < rBE + life) {
				cout <<"Game over..."<<endl;
			break;
			}
			xBE += dxBE;
			yBE += dyBE;
		}
// Random radius with touch and bullet destruction
		if (i == 3) {
			putText (game, "Level 4", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			radius[i] = (radius[i] + 3)% 100;
			xE[i] = width/2 +150;
			yE[i] = height/2;
			if (xBE < 0 || xBE > width || yBE < 0 || yBE > height) {
				xBE = xE[i];
				yBE = yE[i];
				dxBE = (dxBE + 3) % 11 - 5;
				do {
					dyBE = -6 + rand()%14;
				} while (dyBE > -3 && dyBE < 5);
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			int distM = round(sqrt(pow(xE[i] - xCOM, 2) + pow (yE[i] - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (distM < radius[i] + shield && available == true) {
				dX = -dX;
				dY = -dY;
				available = false;
			}
			if (available == false) {
				if ( distE < rBE + life || distM < radius[i] + life) {
					cout <<"Game over..."<<endl;
				break;
				}
			}
			xBE += dxBE;
			yBE += dyBE;
		}

// Bounce and shoot, destroy by bullets and touch
		if (i == 4) {
			putText (game, "Level 5", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			radius[i] = 75;
			xE[i] += dX*2;
			yE[i] += dY*2;
			if (xE[i] - radius[i] < 0) {
				xE[i] = radius[i];
				dX = -dX;
			}
			if (xE[i] + radius[i] > width) {
				xE[i] = width - radius[i];
				dX = -dX;
			}
			if (yE[i] - radius[i] < 0) {
				yE[i] = radius[i];
				dY = -dY;
			}
			if (yE[i] + radius[i] > height) {
				yE[i] = height - radius[i];
				dY = -dY;
			}
			if (xBE < 0 || xBE > width || yBE < 0 || yBE > height) {
				xBE = xE[i];
				yBE = yE[i];
				dxBE = (dxBE + 1) % 11 - 5;
				do {
					dyBE = -5 + rand()%11;
				} while (dyBE > -2 && dyBE < 3);
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			int distM = round(sqrt(pow(xE[i] - xCOM, 2) + pow (yE[i] - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (distM < radius[i] + shield && available == true) {
				dX = -dX;
				dY = -dY;
				available = false;
			}
			if (available == false) {
				if ( distE < rBE + life || distM < radius[i] + life) {
					cout <<"Game over..."<<endl;
				break;
				}
			}
			xBE += dxBE;
			yBE += dyBE;
		}

		int dist = round(sqrt(pow(xBullet - xE[i], 2) + pow (yBullet - yE[i], 2)));
		if (dist < radius[i] + 5) {
			i++;
		}

		circle (game, Point(xCOM, yCOM), life, Scalar (0,0, 255), 3);
		circle (frame, Point(xCOM, yCOM), life, Scalar (0,0, 255), 3);
		if (available) {
			circle (game, Point (xCOM, yCOM), shield, Scalar (255,255,255), 3);
			circle (frame, Point (xCOM, yCOM), shield, Scalar (255,255,255), 3);
		}
		imshow("Raw Image", frame);
		//imshow ("Difference fr Reference", diffRef);
		//imshow ("Difference fr Previous", diffPrev);
		imshow ("Game", game);

		if (waitKey(20) >= 0) {
			break;
		}
	}

	printf("Final frameCount = %d \n", frameCount);
	return 0;
}
