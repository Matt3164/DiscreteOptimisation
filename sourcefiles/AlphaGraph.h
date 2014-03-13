/*
 * AlphaGraph.h
 *
 *  Created on: 12 mars 2014
 *      Author: matthieulegoff
 */




/*
 * StereoGraph.h
 *
 *  Created on: 11 mars 2014
 *      Author: matthieulegoff
 */

#ifndef ALPHAGRAPH_H_
#define ALPHAGRAPH_H_

#include "graph.h"
#include "Node.h"
#include <cv.h>
#include <highgui.h>

typedef Graph <int, int ,int> GraphType;


class AlphaGraph {

public:

	AlphaGraph(){
		TailleX = 2;
		TailleY = 2;
		TailleZ = 2;
		AlphaG = new GraphType(TailleX*TailleY*TailleZ,TailleX*TailleY*TailleZ);
		disparityTable = cv::Mat(TailleX,TailleY, CV_8UC1, cv::Scalar(0));
		energy = std::numeric_limits<int>::max();
		update = false;
	}
	AlphaGraph(GraphType* Graph){
		AlphaG = Graph;
		disparityTable = cv::Mat(TailleX,TailleY, CV_8UC1, cv::Scalar(0));
	}

	void LoadImage(cv::Mat Image, int right);
	void setParameter(int deltaI, int le, int lne);
	void UpdateGraph();
	void CreateGraph();
	int isPixel(int x, int y);
	int IndexPixel(int x, int y);
	int Index(int x, int y);
	void InitNode(int x, int y, int alpha);
	void Initialize(int alpha);
	void updateHorizI(int x, int y, int alpha);
	void updateHorizJ(int x, int y, int alpha);
	void Update(int alpha);
	int Solve();

	void initDisparity();
	int disparity(int x, int y);
	void updateDisparity(int alpha);
	void createDisparityImage();

	int NodeIndex(int x, int y);

	void DoAlphaExpansion(int alpha);


private:

	GraphType* AlphaG;
	cv::Mat ImageLeft;
	cv::Mat ImageRight;

	cv::Mat disparityTable;

	int TailleX;
	int TailleY;
	int TailleZ;

	int DeltaI;
	int lambdaedge;
	int lambdanonedge;

	int energy;
	bool update;

};


#endif /* STEREOGRAPH_H_ */
