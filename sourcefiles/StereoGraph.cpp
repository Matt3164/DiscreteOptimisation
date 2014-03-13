/*
 * StereoGraph.cpp
 *
 *  Created on: 11 mars 2014
 *      Author: matthieulegoff
 */

#include "graph.h"
#include <cv.h>
#include <highgui.h>
#include <opencv2/core/core.hpp>
#include "Stereograph.h"
#include <strstream>

/*CV_LOAD_IMAGE_GRAYSCALE, 0*/


int Norme(cv::Vec3b X, cv::Vec3b Y){
	int sum = 0;

	for(int i = 0; i<3;i++){
		sum+= abs((int) X[0] - (int) Y[0]);
	}
	return sum;
}

int NormeU(int x, int y){

	return abs( x - y);
}

void StereoGraph::LoadImage(cv::Mat Image, int right){
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

}

void StereoGraph::UpdateGraph(){

	Stereo = new GraphType(TailleX*TailleY*TailleZ, 3*TailleX*TailleY*TailleZ);
}

void StereoGraph::CreateGraph(){
	for(int i = 0; i < TailleX*TailleY*TailleZ; i++){
		Stereo -> add_node();
	}
}

void StereoGraph::InitNode(int x, int y, int d){

	if(d == 0){
		int diff = NormeU(ImageLeft.at<uchar>(y,x), ImageRight.at<uchar>(y,x));
		Stereo -> add_tweights(NodeIndex(x,y,d),diff,0);
	}else if(d == TailleZ - 1){
		Stereo -> add_tweights(NodeIndex(x,y,d),0,std::numeric_limits<int>::max());
	}else{
		Stereo -> add_tweights(NodeIndex(x,y,d),0,0);
	}
}

void StereoGraph::Initialize(){

	for(int d = 0; d < TailleZ; d++){
		for(int x = 0; x < TailleX; x++){
			for(int y = 0; y < TailleY; y++){
				InitNode(x,y,d);
			}
		}
	}
}


int StereoGraph::NodeIndex(int x, int y , int d){
	return d * TailleX*TailleY + x*TailleY + y;
}

void StereoGraph::updateVert(int x, int y, int d){
	if(d < TailleZ - 1 && x - d >= 1){
		int diff = NormeU((int) ImageLeft.at<uchar>(y,x), (int) ImageRight.at<uchar>(y ,x-d-1));
		Stereo -> add_edge(NodeIndex(x,y,d), NodeIndex(x,y,d+1),diff,diff);
	}
}

void StereoGraph::updateHorizI(int x, int y, int d){
	if(x < TailleX - 1){
		int w1 = 0;
		if(NormeU((int) ImageLeft.at<uchar>(y,x), (int) ImageRight.at<uchar>(y,x+1)) > DeltaI  ){
			w1 = lambdaedge;
		}else{
			w1 = lambdanonedge;
		}
		Stereo -> add_edge(NodeIndex(x,y,d), NodeIndex(x+1,y,d),w1,w1);
	}
}

void StereoGraph::updateHorizJ(int x, int y, int d){
	if(y < TailleY - 1){
		int w1 = 0;
		if(NormeU((int) ImageLeft.at<uchar>(y, x), (int) ImageRight.at<uchar>(y+1,x)) > DeltaI  ){
			w1 = lambdaedge;
		}else{
			w1 = lambdanonedge;
		}
		Stereo -> add_edge(NodeIndex(x,y,d), NodeIndex(x,y+1,d),w1,w1);
	}
}

void StereoGraph::Update(){

	for(int d = 0; d < TailleZ; d++){
		for(int x = 0; x < TailleX; x++){
			for(int y = 0; y < TailleY; y++){
				updateVert(x,y,d);
				updateHorizI(x,y,d);
				updateHorizJ(x,y,d);
			}
		}
	}
}

int StereoGraph::Solve(){
	return Stereo -> maxflow();
}

int StereoGraph::disparity(int x, int y){
	int disp = 0;
	while(Stereo -> what_segment(NodeIndex(x,y,disp)) == GraphType::SOURCE && disp < TailleZ){
		disp++;
	}
	return disp;
}

void StereoGraph::createDisparityImage(){

	cv::Mat Disparity(TailleX, TailleY , CV_8UC1, cv::Scalar(0));

	for(int x= 0; x < TailleX; x++){
		for(int y = 0; y< TailleY; y++){
			Disparity.at<uchar>(y,x) = 16*disparity(x,y);
			if( y > TailleX){
				std::cout << "Point : " << x << " " << y << " d = " << disparity(x, y) << " update : " << (int) Disparity.at<uchar>(x, y) << " / " << 16*disparity(x, y) << std::endl;
			}

		}
	}

/*	std::stringstream ss;
	ss << "images/Res-di" << DeltaI << "-le" << lambdaedge << "-lne" << lambdanonedge << ".jpg" << std::endl;
	std::vector<int> compression_params; //vector that stores the compression parameters of the image

    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique

    compression_params.push_back(98); //specify the compression quality
	imwrite(ss.str(), Disparity, compression_params);*/


	cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
	imshow("Display Image : ", Disparity);
	 cv::waitKey(0);
}
void StereoGraph::setParameter(int deltaI, int le, int lne){
	DeltaI = deltaI;
	lambdaedge = le;
	lambdanonedge = lne;
}
