/*
 * main.cpp
 *
 *  Created on: 9 mars 2014
 *      Author: matthieulegoff
 */


#include <iostream>
#include "graph.h"
#include <limits>
#include <cv.h>
#include <highgui.h>
#include "StereoGraph.h"
#include "AlphaGraph.h"

using namespace cv;

int Norme1(Vec3b X, Vec3b Y){
	int sum = 0;

	for(int i = 0; i<3;i++){
		sum+= abs((int) X[0] - (int) Y[0]);
	}
	return sum;
}

int main(int argc, char** argv){

	std::cout << "Stereo Matching by Max Flow-Min Cut" << std::endl;

	Mat imageLeft = imread(argv[1], 0);

	Mat imageRight = imread(argv[2], 0);

	int part = 1;

	switch(part){
	case 0:
		{StereoGraph* StereoGr = new StereoGraph;
		StereoGr -> LoadImage(imageLeft,0);
		StereoGr -> LoadImage(imageRight,1);
		StereoGr -> setParameter(10, 5, 10);
		StereoGr -> UpdateGraph();
		StereoGr -> CreateGraph();
		StereoGr -> Initialize();
		StereoGr -> Update();
		std::cout << "Resultat : le flow est de : " << StereoGr -> Solve() << std::endl;

		StereoGr -> createDisparityImage();
		break;}

	case 1:
		{AlphaGraph* AlphaGr = new AlphaGraph;
		AlphaGr -> LoadImage(imageLeft,0);
		AlphaGr -> LoadImage(imageRight,1);
		AlphaGr -> setParameter(8, 20, 40);
		AlphaGr -> initDisparity();
		AlphaGr -> DoAlphaExpansion(3);
		break;}
	}




}

