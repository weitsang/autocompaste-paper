#include "Imgpros.h"
#include <stdio.h>


imgpros::imgpros()
{	
	nb_lines = new unsigned int;
	startt = 0;
	movement = false;
	blankscrn = 0;
	startocr = -1;
	prntcounter = 0;
}

// Applies affine transformation the src image contained in a Mat object given the angle of rotation
void imgpros::rotate(Mat src, Mat &dst, double angle)
{
	Size src_sz = src.size();
	Size dst_sz(src_sz.height, src_sz.width); 

	int len = std::max<int>(src.cols, src.rows); 
	Point2f center(len/2., len/2.);
	Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
	warpAffine(src, dst, rot_mat, dst_sz);
}

// Applies affine transformation the src image contained in a GpuMat object given the angle of rotation
void imgpros::rotate(GpuMat src, GpuMat &dst, double angle)
{
	Size src_sz = src.size();
	Size dst_sz(src_sz.height, src_sz.width); 

	int len = std::max<int>(src.cols, src.rows); 
	Point2f center(len/2., len/2.);
	Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
	warpAffine(src, dst, rot_mat, dst_sz);
}

// Displays the image in the specified window
void imgpros::display(Mat src)
{
	imshow("result", src);
}

void imgpros::display(GpuMat src)
{
	imshow("result", src);
}

// Rotate before displaying
void imgpros::display()
{
	Mat src;
	mattimage.copyTo(src);
	Mat dst;

	double angle = 90;  // or 270
	Size src_sz = src.size();
	Size dst_sz(src_sz.height, src_sz.width); 

	int len = std::max<int>(src.cols, src.rows); 
	Point2f center(len/2., len/2.);
	Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
	warpAffine(src, dst, rot_mat, dst_sz);
	imshow("result", dst);
}

void imgpros::process_main()
{
	webCam>>orig; // gets a new frame from webCam
	// cvtColor changes an image from one color space to another 
	cvtColor(orig, mattimage, CV_BGR2GRAY); // converts orig captured from web cam to gray and stores in mattimage
}

int imgpros::init_camera(int width, int height){
	
	sizeres = cvSize(width, height);

	webCam.release(); // closes video file or capturing device(webcam)
	webCam.open(0); // open the default camera
    if(!webCam.isOpened())  // check if we succeeded
    {
		printf("cannot find webcam ...\n");
		getchar();
		return(-1);
	}
	
	webCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
	webCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);


	//IPL Code
	//CvCapture* p_Webcam;						//asign web cam video stream
	//p_Webcam = cvCaptureFromCAM(0);

	//cvSetCaptureProperty(p_Webcam,CV_CAP_PROP_FRAME_WIDTH,width);
	//cvSetCaptureProperty(p_Webcam,CV_CAP_PROP_FRAME_HEIGHT,height);

	return 1;
}


void imgpros::deskew(Mat img, double angle, Mat &rotated)
{	
	cv::bitwise_not(img, img);
	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	cv::Mat_<uchar>::iterator end = img.end<uchar>();
	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());
		
	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
	cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);
	cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);
	cv::Size box_size = box.size;
	if (box.angle < -45.)
	std::swap(box_size.width, box_size.height);

	cv::Mat cropped;
	cv::getRectSubPix(rotated, box_size, box.center, rotated);
	cv::bitwise_not(rotated, rotated);
	cout<<"deskewing"<<endl;
	
}

//stores frames for averaging
bool imgpros::imagesum(Mat src, int size, vector<Mat> &Tarray)
{	
	Mat tmp;
	src.copyTo(tmp);
	if(Tarray.size() < size)
	{
		Tarray.push_back(tmp);
		return true;
	}
	else
		return false;
}


bool imgpros::imagesum(GpuMat src, int size, vector<GpuMat> &Tarray)
{	
	GpuMat tmp;
	src.copyTo(tmp);
	if(Tarray.size() < size)
	{
		Tarray.push_back(tmp);
		return true;
	}
	else
		return false;
}

//image processing phase 1
int imgpros::process_image(Mat image){
	
//for morphological operations 
//	int morph_elem = 0;
//	int morph_size = 1;
//	int morph_operator = 0;
	
//	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
//	medianBlur ( image, image, 3 );
//	morphologyEx( image, image, MORPH_OPEN, element );
//	morphologyEx( image, image, MORPH_CLOSE, element );
	
	GaussianBlur(image, image, Size(3,3), 0, 0);	
	adaptiveThreshold(image, image, 255.f,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 5, 4.1f);
	return 1;
	}

//resize Mat to different resolution
void imgpros::resizeimage(Mat src,Mat &dst,int width,int height)
{
	if(src.size().width!=width)
		cv::resize(src,dst,cvSize(width,height),0,0,cv::INTER_CUBIC);
	else
		src.copyTo(dst);
}
void imgpros::resizeimage(GpuMat src,GpuMat &dst,int width,int height)
{
	if(src.size().width!=width)
		resize(src,dst,cvSize(width,height),0,0,cv::INTER_CUBIC);
	else
		src.copyTo(dst);
}

//calculate and draw lines with hough transform
void imgpros::callines(Mat src, Mat &dst,int thresh,unsigned int *nb,int thickness,double &Fangle,double acute)
{	
	std::vector<cv::Vec4i> lines;
	
	vector<double> textlines;
	int against = 0;
	Fangle=0;
	//acute = 50;

	cv::HoughLinesP(src, lines, 1, CV_PI/180,thresh, src.size().width/4.f, 10);
	Mat cdst =  Mat::zeros(src.size(),CV_8UC1);;

	
	//cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
	double lineangle,angle = 0.0;
	*nb = lines.size();
	for (unsigned i = 0; i < *nb; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
				(double)lines[i][2] - lines[i][0]);
		Fangle +=lineangle;
	}
	
	Fangle /=(*nb);// mean angle, in radians.
	Fangle *= 180/M_PI;

	//if(Fangle < 10||Fangle >-10)
	for (unsigned i = 0; i < *nb; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
					(double)lines[i][2] - lines[i][0]);
		lineangle *= 180/M_PI;

				if(lineangle>Fangle-acute&&lineangle<Fangle+acute)
		cv::line(cdst, cv::Point(lines[i][0], lines[i][1]),
			cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 255 ,255),thickness,CV_AA);

	}
	cdst.copyTo(dst);
}

void imgpros::callines(GpuMat src, GpuMat &dst,int thresh,unsigned int *nb,int thickness,double &Fangle,double acute)
{	
	std::vector<cv::Vec4i> lines;
	
	vector<double> textlines;
	int against = 0;
	Fangle=0;
	//acute = 50;
	
	gpu::GpuMat Glines;
	//Gsrc.upload(src);
	gpu::HoughLinesBuf Buffer;
	gpu::HoughLinesP(src, Glines, Buffer,1, CV_PI/180,thresh, src.size().width/4.f, 10);
	//cv::HoughLinesP(src, lines, 1, CV_PI/180,thresh, src.size().width/4.f, 10);
	Mat cdst =  Mat::zeros(src.size(),CV_8UC1);;


	        lines.resize(Glines.cols);
		    cv::Mat temp_Mat (1, Glines.cols, CV_32SC4, &lines[0]);
		    Glines.download (temp_Mat);


	
	//cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
	double lineangle,angle = 0.0;
	*nb = lines.size();
	for (unsigned i = 0; i < *nb; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
				(double)lines[i][2] - lines[i][0]);
		//if(lineangle <10||lineangle >-10)
		{
			Fangle +=lineangle;
			//against++;
		}
		//else
			//against++;
	}

	Fangle /=(*nb);// mean angle, in radians.
	Fangle *= 180/M_PI;


	//if(Fangle < 10||Fangle >-10)
	for (unsigned i = 0; i < *nb; ++i)
	{
		lineangle = atan2((double)lines[i][3] - lines[i][1],
					(double)lines[i][2] - lines[i][0]);
		lineangle *= 180/M_PI;

				if(lineangle>Fangle-acute&&lineangle<Fangle+acute)
		cv::line(cdst, cv::Point(lines[i][0], lines[i][1]),
			cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 255 ,255),thickness,CV_AA);

	}
	//cout<<Fangle<<endl;
	//angle = angle*180;
	//Fangle = angle/M_PI;
	cdst.copyTo(dst);

}

void imgpros::cvFillHoles(cv::Mat &input)
{
    //assume input is uint8 B & W (0 or 1)
    //this function imitates imfill(image,'hole')
    cv::Mat holes=input.clone();
    cv::floodFill(holes,cv::Point2i(0,0),cv::Scalar(1));
    for(int i=0;i<input.rows*input.cols;i++)
    {
        if(holes.data[i]==0)
            input.data[i]=1;
    }
}

bool compare(vector<Point2i > a,vector<Point2i > b)
{ return (a.size()>b.size());}

int imgpros::compute_skewg(GpuMat src)
{
	if(startocr == -1)
	{
		startt = omp_get_wtime();
		startocr = 1;
	}
	else
		startocr = 0;

	GpuMat cdstr;
	double mag=0;

	GpuMat cdst;
	bitwise_not(src, src);
	resizeimage(src,cdst,gWidth,gHeight);
	resizeimage(summage,summage,gWidth,gHeight);
	
//	Canny(src,src, 40, 120,3);
//	cv::bitwise_not(cdst, cdst);
	angle = 90;
	//collect good lines
	if(!imagesum(cdst,nPasses,sumofgarray))
	{
		
		while(!sumofgarray.empty())
		{
			bitwise_or(sumofgarray.back(),summage,summage);
			sumofarray.pop_back();
		}
		sumofarray.clear();
		//compare good lines
		if(!imagesum(gsummage,2,difgarray))
		{
			GpuMat a,b,c,error,or,and,xor;
			double werror=0,white;

			difgarray.back().copyTo(c);
			difgarray.back().copyTo(b);
			difgarray.pop_back();


			difgarray.back().copyTo(a);
			
			
		
			bitwise_and(a,c,or);
			bitwise_xor(a,c,error);
			imshow("a",a);

			white = countNonZero(or);
			werror = countNonZero(error);
			imshow("error",error);
			/*
		
			cv::Mat output1 = cv::Mat::zeros(c.size(), CV_8UC3);
			cv::Mat output2 = cv::Mat::zeros(c.size(), CV_8UC3);
			std::vector < std::vector<cv::Point2i > > bloba, blobc;

			
			
			cv::threshold(a, a, 0.0, 1.0, cv::THRESH_BINARY);
			cv::threshold(c, c, 0.0, 1.0, cv::THRESH_BINARY);

			FindBlobs(a,bloba);
			FindBlobs(c,blobc);

			unsigned char red = 255 * (rand()/(1.0 + RAND_MAX));
			unsigned char green = 255 * (rand()/(1.0 + RAND_MAX));
			unsigned char blue = 255 * (rand()/(1.0 + RAND_MAX));


			if(!bloba.empty())
			{
				std::sort(bloba.begin(),bloba.end(),[](vector<Point2i > a,vector<Point2i > b){ return (a.size()<b.size());});
				std::sort(blobc.begin(),blobc.end(),[](vector<Point2i > a,vector<Point2i > b){ return (a.size()<b.size());});

				for(size_t k=0; k < bloba.back().size(); k++) {
					
					int x = bloba.back()[k].x;
					int y = bloba.back()[k].y;

					output1.at<cv::Vec3b>(y,x)[0] = blue;
					output1.at<cv::Vec3b>(y,x)[1] = green;
					output1.at<cv::Vec3b>(y,x)[2] = red;
					
				}
				for(size_t j=0; j < blobc.back().size(); j++) {

					int x = blobc.back()[j].x;
					int y = blobc.back()[j].y;

					output2.at<cv::Vec3b>(y,x)[0] = blue;
					output2.at<cv::Vec3b>(y,x)[1] = green;
					output2.at<cv::Vec3b>(y,x)[2] = red;
				
				}
			
		//		cout<<matchShapes(a,c,CV_CONTOURS_MATCH_I2,0)<<endl;
				//cout<<matchShapes(bloba.back(),blobc.back(),CV_CONTOURS_MATCH_I2,0)<<endl;
				imshow("bloba",output1);
				imshow("blobb",output2);
			}
			*/
			//cout<<"\b";

			cout<<"error\t"<< werror/white*100<<"%      "<<(omp_get_wtime()-startt)<<"\n";
			if(werror/white>=0.40&&!movement)
			{
				movement = true;

			}
			else if(werror/white<0.40&&!movement)
			{
				cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
				difgarray.pop_back();
				difgarray.push_back(b);
				
				
			}
			else if(werror/white<0.40&&movement)
			{
				cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
				difgarray.pop_back();
				difgarray.push_back(b);
				movement=false;
				
			}
			else
			{
				double error;
				if (std::abs(countNonZero(a))>std::abs(countNonZero(c)))
					error = std::abs((double)countNonZero(c))/std::abs((double)countNonZero(a));
				else
					error = std::abs((double)countNonZero(a))/std::abs((double)countNonZero(c));
				
				movement=false;
				cout<<"\nerror white diff :"<<1-error<<"\n";
				
				while(!difgarray.empty())
					difgarray.clear();
				while(!sumofarray.empty())
					sumofgarray.clear();
		

				if(1-error>0.15)
				startocr = 1;
			}

			
			startt = omp_get_wtime();
		}
		//a or b - a and b


	//	rotate(summage,cdstr,90);
	//	imshow("ahhsahd",cdstr);
		
		//reset the sum
		
			//gsummage = GpuMat::zeros(summage.size(),CV_8UC1);
		
	}



	bitwise_not(src, src);
	
		return startocr;
	
}


//            
int imgpros::compute_skew(const Mat src)
{
	if(startocr == -1)
	{
		startt = omp_get_wtime();
		startocr = 1;
	}
	else
		startocr = 0;

	Mat cdstr;
	

	double mag=0;

	

	cv::Size size = src.size();

	Mat cdst = Mat::zeros(size,src.channels());

	cv::bitwise_not(src, src);

	std::vector<cv::Vec4i> lines;
	vector<double> textlines;

	resizeimage(src,cdst,gWidth,gHeight);
	resizeimage(summage,summage,gWidth,gHeight);
	



	angle = 90;
	callines(src,cdst,100,nb_lines,3,angle,20);
	
	//store for frame averaging
	if(!imagesum(cdst,nPasses,sumofarray))
	{
		
		while(!sumofarray.empty())
		{
			bitwise_or(sumofarray.back(),summage,summage);
			sumofarray.pop_back();
		}
		sumofarray.clear();
		
		//compare 2 averaged frames
		if(!imagesum(summage,2,difarray))
		{
			Mat a,b,c,error,or,and,xor;
			double werror=0,white;

			difarray.back().copyTo(c);	//current frame
			difarray.back().copyTo(b);	//
			difarray.pop_back();
			
			difarray.back().copyTo(a);	//previous frame, assumed to be stationary

			bitwise_and(a,c,or);

			bitwise_xor(a,c,error);

			imshow("a",a);
			cvWaitKey(20);
			white = countNonZero(or);
			werror = countNonZero(error);
			imshow("error",error);
			cvWaitKey(20);
			cout<<"error\t"<< werror/white*100<<"%      "<<(omp_get_wtime()-startt)<<"\n";
			//check if there has been any movement, if there is check it again.
			//1st frame is left on stack to compare 1st & 3rd
			if(werror/white>=0.40&&!movement)
			{
				movement = true;

			}
			//no change in frame
			//pop 1st frame from stack and use current(2nd) frame as 1st frame for next iteration
			else if(werror/white<0.40&&!movement)
			{
				cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
				difarray.pop_back();
				difarray.push_back(b);
				
				
			}
			//catch false alarms
			//pop 1st frame from stack and use current(3rd) frame as 1st frame for next iteration
			else if(werror/white<0.40&&movement)
			{
				cout<<" no error :\n";//<< howwhite(error)*100<<"%      "<<(omp_get_wtime()-startt)*1000<<"ms";
				difarray.pop_back();
				difarray.push_back(b);
				movement=false;
				
			}
			//confirmation of page turn
			else
			{
				double error;
				if (std::abs(countNonZero(a))>std::abs(countNonZero(c)))
					error = std::abs((double)countNonZero(c))/std::abs((double)countNonZero(a));
				else
					error = std::abs((double)countNonZero(a))/std::abs((double)countNonZero(c));
				
				movement=false;
				cout<<"\nerror white diff :"<<1-error<<"\n";
				
				while(!difarray.empty())
					difarray.clear();
				while(!sumofarray.empty())
					sumofarray.clear();
		

				if(1-error>0.12)
				startocr = 1;
			}

			
			startt = omp_get_wtime();
		}


		
		//reset the sum
		summage = Mat::zeros(summage.size(),CV_8UC1);
		
	}



	cv::bitwise_not(src, src);

	return startocr;
	
}


//returns percentage of image is white
double imgpros::howwhite(Mat img)
{
	double white=0;
	double black = img.rows*img.cols;

	white = countNonZero(img);
	

	return white/black; 

}

//find large isolated blobs within an image
void imgpros::FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs)
{
    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    cv::Mat label_image;
    binary.convertTo(label_image, CV_32SC1);

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < label_image.rows; y++) {
        int *row = (int*)label_image.ptr(y);
        for(int x=0; x < label_image.cols; x++) {
            if(row[x] != 1) {
                continue;
            }

            cv::Rect rect;
            cv::floodFill(label_image, cv::Point(x,y), label_count, &rect, 0, 0, 4);

            std::vector <cv::Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                int *row2 = (int*)label_image.ptr(i);
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(row2[j] != label_count) {
                        continue;
                    }

                    blob.push_back(cv::Point2i(j,i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
}
