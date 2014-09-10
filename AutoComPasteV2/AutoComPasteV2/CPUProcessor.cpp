//
//  CPUProcessor.cpp
//  AutoComPasteV2
//
//  Created by Raghav Ramesh on 5/9/14.
//  Copyright (c) 2014 FYP. All rights reserved.
//

#include "CPUProcessor.h"

class CPUProcessor : public Processor {
	void display(Mat src) {
		imshow("result", src);
	}

	void rotateClockwise(double angle) {
		Size src_sz = src.size();
	    Size dst_sz(src_sz.height, src_sz.width); 

	    int len = std::max<int>(src.cols, src.rows); 
	    Point2f center(len/2., len/2.);
	    Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
	    warpAffine(src, dst, rot_mat, dst_sz);
	}
	
	bool storeFramesForAveraging(Mat src, int size, vector<Mat> &Tarray) {
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

	void resizeImage(int width, int height) {
		if(src.size().width!=width)
	        cv::resize(src,dst,cvSize(width,height),0,0,cv::INTER_CUBIC);
	    else
	        src.copyTo(dst);
	}

	void applyHoughTransform(int threshold, unsigned int *nb, int thickness, double &Fangle, double acute) {
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

	int computeSkewOfTextInImage(const Mat src) {
		if(startocr == -1)
	    {
	        startt = omp_get_wtime();  //Returns a value in seconds of the time elapsed from some point.
	        startocr = 1;
	    }
	    else
	        startocr = 0;

	    Mat cdstr;
	    

	    double mag=0;

	    

	    cv::Size size = src.size();

	    Mat cdst = Mat::zeros(size,src.channels()); // Fills the matrix with zeros

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

	            difarray.back().copyTo(c);  //current frame
	            difarray.back().copyTo(b);  //
	            difarray.pop_back();
	            
	            difarray.back().copyTo(a);  //previous frame, assumed to be stationary

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
	        summage = Mat::zeros(summage.size(),CV_8UC1); // Fills the matrix with zeros
	        
	    }



	    cv::bitwise_not(src, src);

	    return startocr;
	}

	void findAndStoreBlobs(const cv::Mat &binary, std::vector<std::vector<cv::Point2i>> &blobs) {
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

	double getWhiteContentRatio(Mat img) {
		double white=0;
	    double black = img.rows*img.cols;

	    white = countNonZero(img);
	    

	    return white/black; 
	}

	void fillHolesWithSpecifiedColor(cv::Mat &input) {
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
}