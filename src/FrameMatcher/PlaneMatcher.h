#ifndef PlaneMatcher_H_
#define PlaneMatcher_H_
//OpenCV
#include "cv.h"
#include "highgui.h"
#include <string>
#include <iostream>
#include <stdio.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/registration/icp.h>
#include <pcl/filters/passthrough.h>
#include <Eigen/Core>

#include "FrameMatcher.h"

using namespace std;

class PlaneMatcher: public FrameMatcher
{
	public:
		float weightLimit;
		vector<FrameMatcher *> matchers;
		
		void addMatcher(FrameMatcher * fm);
		PlaneMatcher();
		~PlaneMatcher();
		Transformation * getTransformation(RGBDFrame * src, RGBDFrame * dst);
		void update();
};

#endif
