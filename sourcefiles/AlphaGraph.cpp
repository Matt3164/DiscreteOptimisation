/*
 * AlphaGraph.cpp
 *
 *  Created on: 12 mars 2014
 *      Author: matthieulegoff
 */

/*
 * AlphaGraph.cpp
 *
 *  Created on: 11 mars 2014
 *      Author: matthieulegoff
 */

#include "graph.h"
#include <cv.h>
#include <highgui.h>
#include <opencv2/core/core.hpp>
#include "StereoGraph.h"
#include "AlphaGraph.h"
#include <strstream>
#include <algorithm>

/*CV_LOAD_IMAGE_GRAYSCALE, 0*/


void AlphaGraph::LoadImage(cv::Mat Image, int right){
	switch(right){
	case  0:
		ImageLeft = Image;
		break;
	case 1:
		ImageRight = Image;
		break;
	}

	TailleX = Image.rows;
	TailleY = Image.cols;
	TailleZ = 16;

	std::cout << "Load Image : " << TailleX << "*" << TailleY  << std::endl;
}

void AlphaGraph::UpdateGraph(){

	std::cout << "Graph reset : ";
	AlphaG = new GraphType((2*TailleX-1)*(2*TailleY-1), 2* TailleX*TailleY);
	std::cout << " [OK]" << std::endl;
}

void AlphaGraph::CreateGraph(){
	std::cout << "Graph creation :";
	for(int i = 0; i < (2*TailleX-1)*(2*TailleY-1); i++){
		AlphaG -> add_node();
	}
	std::cout << " [OK]" << std::endl;
}

void AlphaGraph::InitNode(int x, int y, int alpha){

	std::cout << "Starting initialisation of node " << x << " " << y << ":";

	if(isPixel(x,y)){

		int diff = NormeU(ImageLeft.at<uchar>(y,x), ImageRight.at<uchar>(y,x - (int) disparityTable.at<uchar>(int(y/2), int(x/2)) / 16));
		int diffalpha = NormeU(ImageLeft.at<uchar>(y,x), ImageRight.at<uchar>(y,x - alpha));
		AlphaG -> add_tweights(NodeIndex(x,y),diff,diffalpha);
	}else{
		if (x % 2 != 0 && x < 2*TailleX - 2){
			int w = 0;
			if(NormeU(ImageLeft.at<uchar>(y,x), ImageRight.at<uchar>(y,x + 1)) > DeltaI){
				w = lambdaedge;
			}else{
				w = lambdanonedge;
			}

			w *= std::min(2, abs((int) disparityTable.at<uchar>(int(y/2), int((x+1)/2)) / 16 - disparityTable.at<uchar>(int(y/2), int((x -1)/2)) / 16));
			AlphaG -> add_tweights(NodeIndex(x,y),0,w);
		}

		if (y % 2 != 0 && y < 2*TailleX - 2){
			int w = 0;
			if(NormeU(ImageLeft.at<uchar>(y,x), ImageRight.at<uchar>(y+1,x)) > DeltaI){
				w = lambdaedge;
			}else{
				w = lambdanonedge;
			}
			w *= std::min(2, abs((int) disparityTable.at<uchar>(int((y+1)/2), int(x/2)) / 16 - disparityTable.at<uchar>(int((y-1)/2), int(x/2))) / 16);
			AlphaG -> add_tweights(NodeIndex(x,y),0,w);
		}
	}

	std::cout << "[OK]" << std::endl;
}

void AlphaGraph::Initialize(int alpha){

	std::cout << "Graph Initialisation ";

	for(int x = 0; x < 2*TailleX - 1; x++){
		for(int y = 0; y < 2*TailleY-1; y++){
			InitNode(x,y, alpha);
			std::cout << "Node " << x << " " << y  << " initialized"<< std::endl;
		}

		//std::cout << "Line " << x << " updated" << std::endl;
	}

	std::cout << " [OK]" << std::endl;

}


int AlphaGraph::NodeIndex(int x, int y){
	return x*(2*TailleY-1) + y;
}

int AlphaGraph::isPixel(int x, int y){

	return (y % 2 == 0)	|| (x % 2 == 0);
}

int AlphaGraph::IndexPixel(int x, int y){

	return x / 2. * TailleY + y / 2.;
}

int AlphaGraph::Index(int x, int y){
	return x * TailleY + y;
}


void AlphaGraph::updateHorizI(int x, int y, int alpha){
	int w = 0;
	if(x < 2*TailleX - 2){
		if(NormeU(ImageLeft.at<uchar>(int(y/2),int(x/2)), ImageRight.at<uchar>(int(y/2),int((x + 1)/2))) > DeltaI){
			w = lambdaedge;
		}else{
			w = lambdanonedge;
		}
		w *= std::min(2, abs(alpha - disparityTable.at<uchar>(int(y/2),int((x+1)/2)) / 16));
		AlphaG -> add_edge(NodeIndex(x,y), NodeIndex(x+1, y),w,w);
	}
}

void AlphaGraph::updateHorizJ(int x, int y, int alpha){
	int w = 0;
	if(y < 2*TailleY - 2){
		if(NormeU(ImageLeft.at<uchar>(int(y/2),int(x/2)), ImageRight.at<uchar>(int((y+1)/2),int(x/2))) > DeltaI){
			w = lambdaedge;
		}else{
			w = lambdanonedge;
		}
		w *= std::min(2, abs(alpha - disparityTable.at<uchar>(int((y+1)/2), int(x/2)) / 16));
		AlphaG -> add_edge(NodeIndex(x,y), NodeIndex(x, y + 1),w,w);
	}
}

void AlphaGraph::Update(int alpha){

	std::cout << "Graph Update : ";

	for(int x = 0; x < TailleX; x++){
		for(int y = 0; y < TailleY; y++){
			updateHorizI(x,y,alpha);
			updateHorizJ(x,y,alpha);
			std::cout << "Node " << x << " " << y << " updated" << std::endl;
		}
	}
	std::cout << " [OK] " << std::endl;

}

int AlphaGraph::Solve(){

	std::cout << "Min Cut Solver : ";
	int Result = AlphaG -> maxflow();
	if(Result < energy){
		update = true;
		energy = Result;
	}

	std::cout << " [OK]" << std::endl;
	return Result;
}

int AlphaGraph::disparity(int x, int y){
	if(isPixel(x,y)){
		return disparityTable.at<uchar>(int(y/2), int(x/2)) / 16;
	}else{
		std::cout << "Ce n'est pas un pixel" << std::endl;
		return 0;
	}
}

void AlphaGraph::updateDisparity(int alpha){
	std::cout << "Checking for update : ..." << std::endl;
	if(update){
		std::cout << "Disparity Map Update : ";
		for(int x = 0; x < TailleX; x++){
			for(int y = 0; y < TailleY; y++){
				if(AlphaG -> what_segment(NodeIndex(2*x,2*y)) == GraphType::SOURCE && x >= alpha){
					disparityTable.at<uchar>(y, x) = 16*alpha;
				}
			}
		}
		std::cout << "[OK]" << std::endl;
	}
	update = false;
}

void AlphaGraph::createDisparityImage(){


/*	std::stringstream ss;
	ss << "images/Res-di" << DeltaI << "-le" << lambdaedge << "-lne" << lambdanonedge << ".jpg" << std::endl;
	std::vector<int> compression_params; //vector that stores the compression parameters of the image

    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique

    compression_params.push_back(98); //specify the compression quality
	imwrite(ss.str(), Disparity, compression_params);*/

	cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
	imshow("Display Image : ", disparityTable);
	 cv::waitKey(0);
}
void AlphaGraph::setParameter(int deltaI, int le, int lne){

	std::cout << "Setting Parameters : ";
	DeltaI = deltaI;
	lambdaedge = le;
	lambdanonedge = lne;
	std::cout << " [OK]" << std::endl;
}

void AlphaGraph::initDisparity(){
	cv::RNG rng;
	std::cout << "Disparity Matrix Initialisation : ";
	for(int x= 0; x < TailleX; x++){
			for(int y = 0; y< TailleY; y++){
				int dalea = rng.uniform(0,15);
				if(x >= dalea){
					disparityTable.at<uchar>(y, x) = 16*dalea;
				}
		}
	}
	std::cout << " [OK]" << std::endl;
}

void AlphaGraph::DoAlphaExpansion(int alpha){

	UpdateGraph();
	CreateGraph();
	Initialize(alpha);
	Update(alpha);
	Solve();
	updateDisparity(alpha);

}




