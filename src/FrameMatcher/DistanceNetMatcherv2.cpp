#include "DistanceNetMatcherv2.h"
#include <vector>

bool comparison_distance (linkData * i,linkData * j) { return (i->distance<j->distance); }

using namespace std;

DistanceNetMatcherv2::DistanceNetMatcherv2(){
	prior = true;
	name = "DistanceNetMatcherv2";
	max_points = 200;
	nr_iter = 15;
	std_dist = 0.01;
	lookup_size = 100000;
	bounds = 1.96*std_dist;
	probIgnore = 0.001;
	movementPerFrame = 0.02;
	p1_rejection = 0.99;
	p2_rejection = 0.20;
	likelihood_rejection = 3000;
	
	lookup = new float[lookup_size];
	lookup_step = 2*bounds/float(lookup_size+1);
	float lookup_size_half = float(lookup_size)/2;
	float norm = 1/(std_dist*sqrt(2*3.14));
	for(int i = 0; i < lookup_size; i++){
		float tmp = (i-lookup_size_half)*lookup_step;
		lookup[i] = norm*exp(-0.5*(tmp*tmp)/(std_dist*std_dist));
	}
}

DistanceNetMatcherv2::DistanceNetMatcherv2(int max_points_, float std_dist_, float bounds_, bool prior_, float movementPerFrame_, float p1_rejection_, float p2_rejection_, float likelihood_rejection_){
	prior = prior_;
	name = "DistanceNetMatcherv2";
	max_points = max_points_;
	nr_iter = 15;
	std_dist = std_dist_;
	lookup_size = 100000;
	bounds = bounds_;
	probIgnore = 0.001;
	movementPerFrame = movementPerFrame_;
	p1_rejection = p1_rejection_;
	p2_rejection = p2_rejection_;
	likelihood_rejection = likelihood_rejection_;
	
	lookup = new float[lookup_size];
	lookup_step = 2*bounds/float(lookup_size+1);
	float lookup_size_half = float(lookup_size)/2;
	float norm = 1/(std_dist*sqrt(2*3.14));
	for(int i = 0; i < lookup_size; i++){
		float tmp = (i-lookup_size_half)*lookup_step;
		lookup[i] = norm*exp(-0.5*(tmp*tmp)/(std_dist*std_dist));
	}
}

DistanceNetMatcherv2::~DistanceNetMatcherv2(){printf("delete DistanceNetMatcherv2\n");}

using namespace std;
const bool debugg_DistanceNetMatcherv2 = false;

Transformation * DistanceNetMatcherv2::getTransformation(RGBDFrame * src, RGBDFrame * dst)
{
	float frame_dist = fabs(src->id - dst->id);	
	float movement = (frame_dist*movementPerFrame)*(frame_dist*movementPerFrame);

	int nr_loop_src = src->keypoints->valid_key_points.size();
	if(nr_loop_src > max_points){nr_loop_src = max_points;}
	int nr_loop_dst = dst->keypoints->valid_key_points.size();
	if(nr_loop_dst > max_points){nr_loop_dst = max_points;}
	
	vector<KeyPoint * > src_keypoints;
	for(int i = 0; i < nr_loop_src; i++){src_keypoints.push_back(src->keypoints->valid_key_points.at(i));}
	vector<KeyPoint * > dst_keypoints;
	for(int i = 0; i < nr_loop_dst; i++){dst_keypoints.push_back(dst->keypoints->valid_key_points.at(i));}
	
	int src_nr_points = src_keypoints.size();
	int dst_nr_points = dst_keypoints.size();
	
	IplImage* img_combine;
	int width;
	int height;
	if(debugg_DistanceNetMatcherv2)
	{	
		IplImage* rgb_img_src 	= cvLoadImage(src->input->rgb_path.c_str(),CV_LOAD_IMAGE_UNCHANGED);
		char * data_src = (char *)rgb_img_src->imageData;
		IplImage* rgb_img_dst 	= cvLoadImage(dst->input->rgb_path.c_str(),CV_LOAD_IMAGE_UNCHANGED);
		char * data_dst = (char *)rgb_img_dst->imageData;
		
		width = rgb_img_src->width;
		height = rgb_img_src->height;
		
		img_combine = cvCreateImage(cvSize(2*width,height), IPL_DEPTH_8U, 3);
		char * data = (char *)img_combine->imageData;
		for (int j = 0; j < height; j++){
			for (int i = 0; i < width; i++){
				int ind = 3*(640*j+i);
				data[3 * (j * (2*width) + (width+i)) + 0] = data_dst[ind +0];
				data[3 * (j * (2*width) + (width+i)) + 1] = data_dst[ind +1];
				data[3 * (j * (2*width) + (width+i)) + 2] = data_dst[ind +2];

				data[3 * (j * (2*width) + (i)) + 0] = data_src[ind +0];
				data[3 * (j * (2*width) + (i)) + 1] = data_src[ind +1];
				data[3 * (j * (2*width) + (i)) + 2] = data_src[ind +2];
			}
		}
		
		for(int i = 0; i < dst_nr_points;i++){cvCircle(img_combine,cvPoint(dst_keypoints.at(i)->point->w + width	, dst_keypoints.at(i)->point->h), 5,cvScalar(0, 255, 0, 0),2, 8, 0);}
		for(int i = 0; i < src_nr_points;i++){cvCircle(img_combine,cvPoint(src_keypoints.at(i)->point->w			, src_keypoints.at(i)->point->h), 5,cvScalar(0, 255, 0, 0),2, 8, 0);}
		cvNamedWindow("combined image", CV_WINDOW_AUTOSIZE );
		cvReleaseImage( &rgb_img_src );
		cvReleaseImage( &rgb_img_dst );

	}

	if(debugg_DistanceNetMatcherv2){printf("src_keypoints.size() = %i, dst_keypoints.size() = %i\n",int(src_keypoints.size()),int(dst_keypoints.size()));}
	
	float ** current_p = new float*[src_nr_points];
	float * src_likelihood = new float[src_nr_points];
	float ** counter = new float*[src_nr_points];

	
	linkData*** dst_links_sort = new linkData**[dst_nr_points]; 
	for(int i = 0; i < dst_nr_points;i++){dst_links_sort[i] = new linkData*[dst_nr_points];}
	
	linkData*** src_links_sort = new linkData**[src_nr_points];
	for(int i = 0; i < src_nr_points;i++){
		src_links_sort[i] = new linkData*[src_nr_points];
		current_p[i] = new float[dst_nr_points];
		counter[i] = new float[dst_nr_points];
	}
	
	
	float multip = 1/float(dst_nr_points);
	Point * zero_p = new Point();
	for(int i = 0; i < src_nr_points;i++){
		Point * src_p = src_keypoints.at(i)->point;
		float src_radius = src_p->distance(zero_p);
		for(int j = 0; j < dst_nr_points;j++){
			Point * dst_p = dst_keypoints.at(j)->point;
			float dst_radius = dst_p->distance(zero_p);
			float radius_diff = src_radius-dst_radius;
			float d = src_keypoints.at(i)->descriptor->distance(dst_keypoints.at(j)->descriptor);
			if(prior){
				current_p[i][j] = exp(-0.5*(radius_diff*radius_diff)/movement)/(0.01f+d);
			}else{
				current_p[i][j] = 1;
			}
			counter[i][j] = 0;
		}
	}
	delete zero_p;
	
	for(int i = 0; i < src_nr_points;i++){
			float sum = 0;
			for(int j = 0; j < dst_nr_points;j++){sum+= current_p[i][j];}
			if(sum!=0){for(int j = 0; j < dst_nr_points;j++){current_p[i][j] = current_p[i][j]/sum;}}
	}
	

	

	for(int i = 0; i < dst_nr_points;i++){
		vector< linkData * > dst_links_vector_i = vector< linkData * >();
		for(int j = i-1; j >= 0;j--){
			linkData * tmp = new linkData;
			tmp->distance = dst_keypoints.at(i)->point->distance(dst_keypoints.at(j)->point);
			tmp->id1 = i;
			tmp->id2 = j;
			dst_links_vector_i.push_back(tmp);
		}
		sort(dst_links_vector_i.begin(),dst_links_vector_i.end(),comparison_distance);
		for(int j = i-1; j >= 0;j--){dst_links_sort[i][j] = dst_links_vector_i.at(j);}
	}

	for(int i = 0; i < src_nr_points;i++){
		vector< linkData * > src_links_vector_i = vector< linkData * >();
		for(int j = i-1; j >= 0;j--){
			linkData * tmp = new linkData;
			tmp->distance = src_keypoints.at(i)->point->distance(src_keypoints.at(j)->point);
			tmp->id1 = i;
			tmp->id2 = j;
			src_links_vector_i.push_back(tmp);
		}
		sort(src_links_vector_i.begin(),src_links_vector_i.end(),comparison_distance);
		for(int j = i-1; j >= 0;j--){src_links_sort[i][j] = src_links_vector_i.at(j);}
	}

	struct timeval start, end;
	gettimeofday(&start, NULL);
	
		
	vector<linkPair* > linkPair_vec;
	vector<int > linkPair_int_vec;
	vector<int * > linkPair_skip_vec;
	const int vlen = 10000;
	linkPair_vec.push_back(new linkPair[vlen]);
	linkPair_int_vec.push_back(0);
	
	int cvpos = 0;
	linkPair * cvec = linkPair_vec.back();
	float minusbounds = -bounds;
	for(int i = 0; i < src_nr_points;i++){
		linkData ** src_links_sort_i =  src_links_sort[i];
		for(int j = 0; j < dst_nr_points;j++){
			if(current_p[i][j] > probIgnore){
				linkData ** dst_links_sort_i =  dst_links_sort[j];
				int last_start = 0;
				int cvpos_start = cvpos;
				int cvec_jump = 0;
				for(int ii = 0; ii < i;ii++){
					linkData * src_ld = src_links_sort_i[ii];
					float src_distance = src_ld->distance;
					int src_id1 = src_ld->id1;
					int src_id2 = src_ld->id2;
					float * current_p_1 = current_p[src_id1];
					float * current_p_2 = current_p[src_id2];
					float * counter_1 = counter[src_id1];
					float * counter_2 = counter[src_id2];
					
					if(vlen-cvpos < j-last_start){
					 	linkPair_vec.push_back(new linkPair[vlen]);
						linkPair_int_vec.push_back(0);
						cvpos = 0;
						cvec = linkPair_vec.back();
						cvec_jump++;
					}
					
					for(int jj = last_start; jj < j; jj++){
						linkData * dst_ld = dst_links_sort_i[jj];	
						float diff_distance = src_distance - dst_ld->distance;
						int dst_id1 = dst_ld->id1;
						int dst_id2 = dst_ld->id2;	
						
						if(diff_distance > minusbounds){
							if(diff_distance < bounds){
								if(current_p_2[dst_id2] > probIgnore){
									linkPair & lp = cvec[cvpos];
									lp.src_link = src_ld;
									lp.dst_link = dst_ld;
									lp.value = lookup[int((0.5f*((diff_distance/bounds)+1.0f))*lookup_size)];
									lp.p_src = current_p_1+dst_id1;
									lp.p_dst = current_p_2+dst_id2;
									lp.c_i = counter_1+dst_id1;
									lp.c_j = counter_2+dst_id2;
									//linkpairs[current_link_pairs] = lp;
									cvpos++;
								}
							}else{last_start++;}
						}else{break;}
					}
					linkPair_int_vec.back() = cvpos;
				}
				int cvpos_stop = cvpos;
				int * skip_tmp = new int[3];
				skip_tmp[0]=cvpos_start;
				skip_tmp[1]=cvpos_stop;
				skip_tmp[2]=cvec_jump;
				//printf("%i %i %i\n",cvpos_start,cvpos_stop,cvec_jump);
				linkPair_skip_vec.push_back(skip_tmp);
			}
		}
	}
	
	gettimeofday(&end, NULL);
	float setup1_time = (end.tv_sec*1000000+end.tv_usec-(start.tv_sec*1000000+start.tv_usec))/1000000.0f;
	printf("setup1 cost: %f\n",setup1_time);
	gettimeofday(&start, NULL);

	for(int iter = 0; iter < nr_iter; iter++){
		gettimeofday(&start, NULL);
		for(int i = 0; i < src_nr_points;i++){for(int j = 0; j < dst_nr_points;j++){counter[i][j] = 0;}}
		linkPair lp;

		/*
		for(int i = 0; i < linkPair_vec.size(); i++){
			linkPair * cvec = linkPair_vec.at(i);
			int max = linkPair_int_vec.at(i);
			for(int j = 0; j < max; j++){
				lp = cvec[j];
				float p1 = (*lp.p_src);
				//if(p1 > probIgnore){
				float val = p1*(*lp.p_dst)*lp.value;
					//if(val > 0.000001){
				(*lp.c_i)+=val;
				(*lp.c_j)+=val;
					//}
				//}
			}
		}
		*/
		
		cvpos = 0;
		int cvec_ind = 0;
		cvec = linkPair_vec.at(cvec_ind);
		int cvec_max = linkPair_int_vec.at(cvec_ind);
		
		for(int i = 0; i < linkPair_skip_vec.size(); i++){
			int cvec_ind_start = cvec_ind;
			int * v = linkPair_skip_vec.at(i);
			int start_ind = v[0];
			int stop_ind = v[1];
			int skip = v[2];
			cvpos = start_ind;
			if(start_ind >= cvec_max){cvpos = start_ind = 0;}
			lp = cvec[cvpos];
			float p1 = (*lp.p_src);
			if(p1 > probIgnore){
				if(skip > 0){
					//first
					for(cvpos = start_ind; cvpos < cvec_max; cvpos++){
						lp = cvec[cvpos];
						float val = p1*(*lp.p_dst)*lp.value;
						(*lp.c_i)+=val;
						(*lp.c_j)+=val;
					}
					cvec_ind++;
					cvec = linkPair_vec.at(cvec_ind);
					cvec_max = linkPair_int_vec.at(cvec_ind);
					cvpos = 0;
					//loops
					for(int ii = 0; ii < skip-1; ii++){
						for(cvpos = 0; cvpos < cvec_max; cvpos++){
							lp = cvec[cvpos];
							float val = p1*(*lp.p_dst)*lp.value;
							(*lp.c_i)+=val;
							(*lp.c_j)+=val;
						}
						cvec_ind++;
						cvec = linkPair_vec.at(cvec_ind);
						cvec_max = linkPair_int_vec.at(cvec_ind);
						cvpos = 0;
					}
					//last
					for(cvpos = 0; cvpos < stop_ind; cvpos++){
						lp = cvec[cvpos];
						float val = p1*(*lp.p_dst)*lp.value;
						(*lp.c_i)+=val;
						(*lp.c_j)+=val;
					}
				}else{
					for(cvpos = start_ind; cvpos < stop_ind; cvpos++){
						lp = cvec[cvpos];
						float val = p1*(*lp.p_dst)*lp.value;
						(*lp.c_i)+=val;
						(*lp.c_j)+=val;
					}
				}
			}else{
				cvec_ind += skip;
				cvec = linkPair_vec.at(cvec_ind);
				cvec_max = linkPair_int_vec.at(cvec_ind);
			}
		}
		for(int i = 0; i < src_nr_points;i++)
		{
			float sum = 0;
			for(int j = 0; j < dst_nr_points;j++){sum+= counter[i][j];}
			src_likelihood[i] = sum;
			if(sum!=0){
				for(int j = 0; j < dst_nr_points;j++){current_p[i][j] = counter[i][j]/sum;}
			}
		}
		gettimeofday(&end, NULL);
		float time1 = (end.tv_sec*1000000+end.tv_usec-(start.tv_sec*1000000+start.tv_usec))/1000000.0f;
		if(debugg_DistanceNetMatcherv2){printf("net cost: %f\n",time1);}
	}
	Transformation * transformation = new Transformation();
	transformation->transformationMatrix = Eigen::Matrix4f::Identity();
	transformation->src = src;
	transformation->dst = dst;
	
	for(int i = 0; i < src_nr_points;i++){
		float best_v = -1;
		int best_i = 0;
		for(int j = 0; j < dst_nr_points;j++){
			if(best_v < current_p[i][j]){
				best_v = current_p[i][j];
				best_i = j;
			}
		}
		Point * src_i = src_keypoints.at(i)->point;
		Point * dst_j = dst_keypoints.at(best_i)->point;
		if(current_p[i][best_i] > p1_rejection && src_likelihood[i] > likelihood_rejection){
			
			float sum = 0;
			for(int ii = 0; ii < src_nr_points;ii++){
				sum += current_p[ii][best_i]*src_likelihood[ii];
			}
			float p2 = src_likelihood[i]/sum;

			if(p2 > p2_rejection){
				if(debugg_DistanceNetMatcherv2){cvLine(img_combine,cvPoint(src_i->w ,src_i->h),cvPoint(dst_j->w+width,dst_j->h),cvScalar(0, 255, 0, 0),1, 8, 0);}
				transformation->matches.push_back(make_pair (src_keypoints.at(i), dst_keypoints.at(best_i)));
			}
		}
	}

	if(debugg_DistanceNetMatcherv2){
		printf("done\n");
		cvShowImage("combined image", img_combine);
		cvWaitKey(0);
		cvReleaseImage( &img_combine);
	}
	
	transformation->weight = transformation->matches.size();
	if(transformation->weight >= 3){
		pcl::TransformationFromCorrespondences tfc;
		for(int i = 0; i < transformation->weight; i++){
			tfc.add(transformation->matches.at(i).first->point->pos, transformation->matches.at(i).second->point->pos);
		}
		transformation->transformationMatrix = tfc.getTransformation().matrix();
	}
	
	//CLEAN
	for(int i = 0; i < dst_nr_points;i++){
		for(int j = i-1; j >= 0;j--){delete dst_links_sort[i][j];}
		delete[] dst_links_sort[i];
	}
	delete[] dst_links_sort;
	
	for(int i = 0; i < src_nr_points;i++){
		delete[] current_p[i];
		delete[] counter[i];
		for(int j = i-1; j >= 0;j--){delete src_links_sort[i][j];}
		delete[] src_links_sort[i];
	}
	delete[] current_p;
	delete[] counter;
	delete[] src_links_sort;
	
	for(int i = 0; i < linkPair_vec.size(); i++)		{delete[] linkPair_vec.at(i);}	
	for(int i = 0; i < linkPair_skip_vec.size(); i++)	{delete[] linkPair_skip_vec.at(i);}
	return transformation;
}
