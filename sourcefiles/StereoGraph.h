/*
 * StereoGraph.h
 *
 *  Created on: 11 mars 2014
 *      Author: matthieulegoff
 */

#ifndef STEREOGRAPH_H_
#define STEREOGRAPH_H_

#include "graph.h"
#include "Node.h"
#include <cv.h>
#include <highgui.h>

typedef Graph <int, int ,int> GraphType;

int Norme(cv::Vec3b X, cv::Vec3b Y);

int NormeU(int x, int y);


class StereoGraph {

public:

	StereoGraph(){
		TailleX = 2;
		TailleY = 2;
		TailleZ = 2;
		Stereo = new GraphType(TailleX*TailleY*TailleZ,TailleX*TailleY*TailleZ);
	}
	StereoGraph(GraphType* Graph){
		Stereo = Graph;
	}

	void LoadImage(cv::Mat Image, int right);
	void setParameter(int deltaI, int le, int lne);
	void UpdateGraph();
	void CreateGraph();
	void InitNode(int x, int y, int d);
	void Initialize();
	void updateVert(int x, int y, int d);
	void updateHorizI(int x, int y, int d);
	void updateHorizJ(int x, int y, int d);
	void Update();
	int Solve();
	int disparity(int x, int y);
	void createDisparityImage();

	int NodeIndex(int x, int y, int d);


private:

	GraphType* Stereo;
	cv::Mat ImageLeft;
	cv::Mat ImageRight;

	int TailleX;
	int TailleY;
	int TailleZ;

	int DeltaI;
	int lambdaedge;
	int lambdanonedge;

};


#endif /* STEREOGRAPH_H_ */
