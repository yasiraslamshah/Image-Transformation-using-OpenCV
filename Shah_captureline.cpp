/*
YASIR ASLAM SHAH
EXAMPLE CODE BY SAM STEWART
Hough Lines TRANSFORM

*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <pthread.h>  
#include <sched.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <semaphore.h>
#include <opencv/highgui.h>

#define FRAMES  100
#define NUM_OF_THREADS  2 
#define VRES  1080
#define HRES  1440

using namespace cv;
using namespace std;

sem_t semA,semB;
struct timespec A_FRAME1,A_FRAME2;

double  CURRENT_A_FRAME =0.0;
double  PREVIOUS_A_FRAME=0.0;

//	double A_AVERAGE_FRAME,A_AVERAGE_RATE,A_FRAME_COUNT,A_FRAME,A_JITTER,A_AVERAGE_POSITIVE_JITTER,A_AVERAGE_NEGATIVE_JITTER,
double TOTAL_JITTER=0.0;
double	A_AVERAGE_FRAME=0.0;
double	A_AVERAGE_RATE=0.0;
double	A_FRAME_COUNT=0.0;
double	A_FRAME=0.0;
double	A_JITTER=0.0;
double	A_AVERAGE_POSITIVE_JITTER=0.0;
double	A_AVERAGE_NEGATIVE_JITTER=0.0;
double c=0.0;
double x=0.0;
double y=0.0;
int i;
int A_FC=0.0;
double temp=0.0;
//Canny Threasholds
vector<Vec4i> lines;


int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;

IplImage* frame;
Mat canny_frame, cdst, timg_gray, timg_grad, canny_frame1;


//Global variable for capture to be used by all threads
CvCapture* capture;

//Write all values in Comments then update acc. to res
//Deadlines set from analysis
double CANNY_DEADLINE = 175.0;




void *CannyTransform(void *a)
{
sem_wait(&semA);
while (i< FRAMES){
frame = cvQueryFrame(capture);
i++;

	if(!frame)	break;

	else{
	A_FC++;//frame count
		if(A_FC > 2)
			{
//			printf("Previous frame= %f\t", PREVIOUS_A_FRAME);
			A_FRAME_COUNT = (double)A_FC;
			A_AVERAGE_FRAME= (((A_FRAME_COUNT -1) * A_AVERAGE_FRAME) + A_FRAME)/A_FRAME_COUNT ;
			A_AVERAGE_RATE= 1.0/(A_AVERAGE_FRAME/1000);
			x=x+A_AVERAGE_RATE;
			y=y+A_AVERAGE_FRAME;

//		printf("avergae rate and avergae frame rate =%f and %f\n ",A_AVERAGE_RATE,A_AVERAGE_FRAME);

			}

		Mat mat_frame(cvarrToMat(frame));
		Canny(mat_frame,canny_frame,50,200,3);

//cvtColor(mat_frame, timg_gray, CV_RGB2GRAY);
		/// Reduce noise with a kernel 3x3

		HoughLinesP(canny_frame,lines,1,CV_PI/180,50,50,10);

for (size_t j=0;j<lines.size();j++)
{
Vec4i l= lines[j];
line(mat_frame,Point(l[0],l[1]),Point(l[2],l[3]),Scalar(0,0,225),3,CV_AA);
}


cvShowImage("Hough Lines",frame);

//		blur( timg_gray, canny_frame, Size(3,3) );
		/// Canny detector
//		Canny( canny_frame, canny_frame, lowThreshold,0, kernel_size );
		/// Using Canny's output as a mask, we display our result
//		timg_grad = Scalar::all(0);
//		mat_frame.copyTo( timg_grad, canny_frame);
//		imshow( "CANNY TRANSFORM\n", timg_grad );

		clock_gettime(CLOCK_REALTIME,&A_FRAME2);
		CURRENT_A_FRAME =( (A_FRAME2.tv_sec*1000.0) + (A_FRAME2.tv_nsec/1000000.0)) ;
		//c= CURRENT_A_FRAME- PREVIOUS_A_FRAME;

		//printf("current frame =%f\n",CURRENT_A_FRAME);
		A_FRAME = CURRENT_A_FRAME - PREVIOUS_A_FRAME;
		PREVIOUS_A_FRAME = CURRENT_A_FRAME;

                      if (A_FC > 2){
         A_JITTER = CANNY_DEADLINE - A_FRAME ;
	temp =A_JITTER;
//         printf("Jitter is %f and deadline is %f\n",A_JITTER,A_FRAME);

                 if(A_JITTER < 0){
                A_AVERAGE_NEGATIVE_JITTER = (A_AVERAGE_NEGATIVE_JITTER + A_JITTER);
             printf("Negative Jitter:	%f. \t Frame  Time :%f \t\t Deadline Missed for Frame %d\n",temp, A_FRAME,i);
//              A_AVERAGE_NEGATIVE_JITTER = (A_AVERAGE_NEGATIVE_JITTER + A_JITTER);
                                }
                else{
                A_AVERAGE_POSITIVE_JITTER = (A_AVERAGE_POSITIVE_JITTER + A_JITTER);

              printf("Positive Jitter:	%f\n", temp);
//                A_AVERAGE_POSITIVE_JITTER = (A_AVERAGE_POSITIVE_JITTER + A_JITTER);
                        }


}
		char X= cvWaitKey(33);
		if ((X == 'x')|(X == 'X')){
		printf( "EXIT\n");}

	sem_post(&semB);
		//printf("\nCANNY TRANSFORM\n");
			//printf("No of Frames Captured		:%d\n",FRAMES);
			//printf("No of Frames Per second	:%F\n",A_AVERAGE_RATE);
			//printf("Average Time rate per Frame	:%f\n",A_AVERAGE_FRAME);
			//printf("Positive Jitter 	:%f 	Negative Jitter	:%f  \n",A_AVERAGE_POSITIVE_JITTER,A_AVERAGE_NEGATIVE_JITTER);
			//printf("Total average Jitter	:%f\n",(TOTAL_JITTER/FRAMES));
}}
}


void *Jitter(void *a){
sem_wait(&semB);

//printf("hi\n");
/* if (A_FC > 2){
	 A_JITTER = CANNY_DEADLINE - A_FRAME ;
         printf("Jitter is %f and deadline is %f\n",A_JITTER,A_FRAME);

		 if(A_JITTER < 0){
                printf("Negative Jitter. Frame  Time :%f \t\t Deadline Missed for Frame %d\n", A_FRAME,i);
              A_AVERAGE_NEGATIVE_JITTER = (A_AVERAGE_NEGATIVE_JITTER + A_JITTER);
				}
                else{
//              printf("POSItive Jitter. Frame Time : %f \td", A_FRAME,i);
                A_AVERAGE_POSITIVE_JITTER = (A_AVERAGE_POSITIVE_JITTER + A_JITTER);
                	}}
//printf("Total Jitter = %f\n",(TOTAL_JITTER/FRAMES));
*/
TOTAL_JITTER=A_AVERAGE_POSITIVE_JITTER+A_AVERAGE_NEGATIVE_JITTER;
printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
			printf("	Total Jitter(ms):	       		 %f\n",(TOTAL_JITTER/FRAMES));
			printf("\n\n	RESOLUTION:  	       		 %d x %d\n",HRES,VRES);
                        printf("	Total number of Frames:          %d\n",FRAMES);
                        printf("	Frames Per Second(FPS):	         %f\n",(x/FRAMES));
                        printf("	Average Time rate per Frame(ms):     %f\n",(y/FRAMES));




sem_post(&semA);
}


	int main ( int argc, char** argv )
	{
		//Semapohore init, init semCanny as 1, so that it can be taken by Canny for the first time
sem_init(&semA,0,1);
sem_init(&semB,0,0);
		//Pthread Attributes and Parameters Init
	pthread_attr_t rt_sched_attr;
pthread_t thread0,thread1;
	//pthread_t threads[2]; 
	struct sched_param rt_param;

		// All threads are set with same priority of 98
		rt_param.sched_priority=98;

		//Pthread Attributes and Parameters Init
		pthread_attr_init(&rt_sched_attr);
		pthread_attr_setinheritsched(&rt_sched_attr,PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&rt_sched_attr,SCHED_FIFO);
		pthread_attr_setschedparam(&rt_sched_attr,&rt_param);

		//Taking cmd lines args from user to select camera device port 0,1.... etc
		int dev=0;
		if(argc > 1)
		{
			sscanf(argv[1], "%d", &dev);
			printf("using %s\n", argv[1]);
		}
		else if(argc == 1)
			printf("using default\n");
		else
		{
			printf("usage: capture [dev]\n");
			exit(-1);
		}
//craeteTrackbar("Min Threshold:","yasir",0,max_lowThreshold,CannyTransform);
		//Capture on camera device with given id
		capture = (CvCapture *)cvCreateCameraCapture(dev);
		//Set resolutions 
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);


		//Threads are created and joined under a while 1 , to provide continous frame transformation.

			pthread_create(&thread0,NULL,CannyTransform,NULL);
pthread_join(thread0,NULL);
			pthread_create(&thread1,NULL,Jitter,NULL);
	///		pthread_create(&threads1,&rt_sched_attr,CannyTransform,NULL);
//			printf("hey");
//CannyTransform(0);
//Jitter();
		//	pthread_join(thread0,NULL);
			pthread_join(thread1,NULL);
//			pthread_join(threads[1],NULL);
	//	}
//}

		cvReleaseCapture(&capture);
}



