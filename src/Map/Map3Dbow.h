#ifndef Map3Dbow_H_
#define Map3Dbow_H_

//other
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include "RGBDFrame.h"
#include "Transformation.h" 

#include "FrameMatcher.h"
#include "FeatureExtractor.h"

#include "Map3D.h"

using namespace std;

class Map3Dbow: public Map3D
{
	public:

	Map3Dbow();
	~Map3Dbow(); 
	
	void addFrame(Frame_input * fi);
	void addFrame(RGBDFrame * frame);
	void addTransformation(Transformation * transformation);
	
	void estimate();
	void setVisualization(boost::shared_ptr<pcl::visualization::PCLVisualizer> view);
	void visualize();
	vector<FeatureDescriptor * > * kmeans(vector<FeatureDescriptor *> input, int nr_restarts, int iterations, int nr_clusters);
};
#endif
