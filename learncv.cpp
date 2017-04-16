// Name        : learncv.cpp
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

	int xE[10], yE[10], b[10], g[10], r[10], radius[10];
	for (i = 0; i < 10; i++) {
		xE[i] = 50 + rand()%(width - 100);
		b[i] = 12 + rand()%230;
		g[i] =  128 + rand()%128;
		r[i] = 50 + rand()%205;
		radius[i] = 5 + rand()% 46;
		yE[i] = radius[i] + rand()%100;

	}
	i = 0;
	for (i = 4; i < 7; i++) {
		xE[i] = 100 + rand()% 300;
		yE[i] = 100 + rand()% 100;
	}

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
		if (i == 0) { // Instruction
			putText (game, "Level 1", Point(10, height - 10),
				FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			putText (game, "Shoot Target!", Point(200, height - 80),
				FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2);
		}
		if (i == 1) { // Move on x-axis and shoot straight downwards
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
			yBE += 4;
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (available == false && distE < rBE + life) {
				cout <<"Game over..."<<endl;
				break;
			}
		}
		if (i == 2) { // Bounce on the horizon and shoot in random direction
			putText (game, "Level 3", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			xE[i] += dX;
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (xBE < 0 || xBE > width || yBE < 0 || yBE > height) {
				xBE = xE[i];
				yBE = yE[i];
				dxBE = (dxBE + 1) % 11 - 5;
				dyBE = 3 + rand()%3;
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);
			xBE += dxBE;
			yBE += dyBE;
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (available == false && distE < rBE + life) {
				cout <<"Game over..."<<endl;
				break;
			}
		}
		if (i == 3) { //Bounce against walls and shoot in random direction
			putText (game, "Level 4", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			xE[i] += dX;
			yE[i] += -dY;
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (yE[i] - radius[i] < 0 || yE[i] + radius[i] > height) {
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
			xBE += dxBE;
			yBE += dyBE;
			int distE = round(sqrt(pow(xBE - xCOM, 2) + pow (yBE - yCOM, 2)));
			if (distE < rBE + shield && available == true) {
				xBE = -3;
				available = false;
			}
			if (available == false && distE < rBE + life) {
				cout <<"Game over..."<<endl;
			break;
			}
		}
		if (i == 4) {
			putText (game, "Level 5", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			radius[i] = 100;
			xE[i] += round(dX*4/3);
			yE[i] += round(dY*3/2);
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (yE[i] - radius[i] < 0 || yE[i] + radius[i] > height) {
				dY = -dY;
			}
			int distM = round(sqrt(pow(xE[i] - xCOM, 2) + pow (yE[i] - yCOM, 2)));
			if (distM < radius[i] + shield && available == true) {
				dX = -dX;
				dY = -dY;
				available = false;
			}
			if (available == false && distM < radius[i] + life) {
				cout <<"Game over..."<<endl;
			break;
			}
		}
		if (i == 5) {
			putText (game, "Level 6", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			radius[i] = 75;
			xE[i] += dX*2;
			yE[i] += dY*2;
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (yE[i] - radius[i] < 0 || yE[i] + radius[i] > height) {
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
			xBE += dxBE;
			yBE += dyBE;
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
		}
		if (i == 6) {
			putText (game, "Level 7", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
			radius[i] = (radius[i] + 3)% 100;
			xE[i] += dX*(rand()%3);
			yE[i] += -(dY + 2)%7;
			if (xE[i] - radius[i] < 0 || xE[i] + radius[i] > width) {
				dX = -dX;
			}
			if (yE[i] - radius[i] < 0 || yE[i] + radius[i] > height) {
				dY = -dY;
			}
			if (xBE < 0 || xBE > width || yBE < 0 || yBE > height) {
				xBE = xE[i];
				yBE = yE[i];
				dxBE = (dxBE + 3) % 11 - 5;
				do {
					dyBE = -6 + rand()%14;
				} while (dyBE > -3 && dyBE < 5);
			}
			circle (game, Point (xBE, yBE), rBE, Scalar (b[i], g[i], r[i]), -1);
			xBE += dxBE;
			yBE += dyBE;
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
		}
		if (i == 7) {
			putText (game, "Level 8", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
		}
		if (i == 8) {
			putText (game, "Level 9", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
		}
		if (i == 9) {
			putText (game, "Level 10", Point(10, height - 10),
					FONT_HERSHEY_PLAIN, 2, Scalar(0,128,255), 2);
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
