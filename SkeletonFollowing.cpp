// SkeletonFollowing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <NuiApi.h>
#include <stdio.h>
#include <iostream>
#include "Serial.h"
#include <math.h>

#define MAX_NUM_USERS 6
#define MIN_PWM 50
#define MIN_PWM_RVR 20
#define MIN_PWM_D 40

#define MAX_NUM_USERS 6

#define LIMIT_UPPER 1300
#define LIMIT_LOWER 1000
#define DIRECTION 200

using namespace std;


// Global Variables
double dist[6 ] = {0,0,0,0,0,0};                    // Distance to the given user ID
double x[6] = {0};                           // Deviation of given user to left or right

double dis[10] = {0, 0, 0, 0,0,0,0,0,0,0};       // Holds 4 previous distance values
double x_[10] = {0, 0, 0, 0,0,0,0,0,0,0};        // Holds diviation information of users


unsigned char pwm_r = 0;                      // Right motor PWM
unsigned char pwm_l = 0;                      // Left Motor PWM 
 

float distance_factor = 0.0426;       // when going normal  128/3000
float distance_factor_sf = 0.0852;     // when going straigt forword 255/3000
float distance_factor_b = 0.318;      // distance factor for reverse 255/600
float direction_factor = 0.167;       // maximum deviation 100/

char stre[4] ; 

void sendData( char direction, int pwm_r, int pwm_l );

// Serial Communication
    Serial* sp = new Serial("\\\\.\\COM29"); // adjust as needed
	int readResult = 0;
	int writeResult = 0;

// End of Serial communication configuration

void stopRobot()
{
		printf("Stopping...\n");
				for( int count = 0; count < 50; count++  )
				sendData('S', 0, 0);            //stop the robot
}

void sendData( char direction, int pwm_r, int pwm_l )
{
	stre[0] = 0;
	stre[1] = direction;
	stre[2] = (pwm_r<127) ? -pwm_r: pwm_r-127 ;
	stre[3] = (pwm_l<127) ? -pwm_l: pwm_l-127;
		  
	writeResult = sp->WriteData(stre,4); // files buffers are flushed in SerialSource App			
}

void move_robot()
{
	/* average distance and X values */
	float value = (dis[0] + dis[1] + dis[2] + dis[3] ) /4.0;
	float direction = ( x_[0] + x_[1] +x_[2] + x_[3] ) / 4.0;

	printf("Value = %f, Direction %f\n", value, direction);
	/**************************************************************************************************************
						FORWORD MOTION

	*************************************************************************************************************/
	
	if( value == 0.0 )
	{
	
	pwm_r = 0;
	pwm_l = 0;
	sendData('S', pwm_r, pwm_l);

		//printf("------%d===%d-------\n",pwm_r, pwm_l);

	}else if( value >= LIMIT_UPPER ) // if distance is greater than LIMIT_UPPER go forword
	{

		/* Move forword */
		if( direction < -DIRECTION  ) /* go right */
		{
			pwm_r = MIN_PWM + distance_factor * ( value - LIMIT_UPPER ) ;
			pwm_l = MIN_PWM + distance_factor * ( value - LIMIT_UPPER ) + 50; //direction_factor * (-direction);
			//pwm_r = 80;
			//pwm_l = 90;
			sendData( 'F', pwm_r, pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);
		 			
		}
		else if( direction >= DIRECTION ) // go left
		{
			pwm_r = MIN_PWM + distance_factor * ( value - LIMIT_UPPER  )+50 ;//+ direction_factor * direction;
			pwm_l = MIN_PWM + distance_factor * ( value - LIMIT_UPPER ) ;
			
			//pwm_r = 90;
			//pwm_l = 80;

			sendData( 'F', pwm_r, pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);	
		}
		else                    // Go straight
		{
			pwm_r = MIN_PWM + distance_factor_sf * ( value - LIMIT_UPPER ) ;
			pwm_l = MIN_PWM + distance_factor_sf * ( value - LIMIT_UPPER ) ;
			//pwm_r = 90;
			//pwm_l = 90;

			sendData('F', pwm_r, pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);	
		}

	}
	/****************************************************************************************************
	*					                            STOP                                               *
	*****************************************************************************************************/



		else if( (value >= LIMIT_LOWER && value < LIMIT_UPPER) )    // stop movement in 800 -- 1000 range        
	{
		
		/* stop the motors */
		if( direction < -DIRECTION  )  // turn right
		{ 
			pwm_r = MIN_PWM_D  ;
			pwm_l = MIN_PWM_D + 30;// + direction_factor *(-direction)  ;
			//pwm_r = 0 ;
			//pwm_l = 50 ;

			sendData('F', pwm_r,pwm_l);
			//printf("------%d    %d---\n",pwm_r, pwm_l);
			
		}
		else if( direction >= DIRECTION  ) // turn left
		{
			pwm_r = MIN_PWM_D +30; //+ direction_factor * direction  ;
			pwm_l = MIN_PWM_D ;
			
			//pwm_r = 50;
			//pwm_l = 0;

		
			sendData('F', pwm_r,pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);
		}else                  // stop motors
		{
			//pwm_r = MIN_PWM_RVR + distance_factor_b * ( 1000 - value ) ;
			//pwm_l = MIN_PWM_RVR + distance_factor_b * ( 1000 - value ) ;
			pwm_r = 0;
			pwm_l = 0;

			sendData('S', 0,0);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);
		}
		
	}

	/*************************************************************************************************
	*							      BACKWORD MOTION                                                *
	**************************************************************************************************/

	else if( value >= 0.8 && value < LIMIT_LOWER  )            // go backword
	    {
		
		
		if( direction < -DIRECTION+100  ) /* go left */
		{
			pwm_r = MIN_PWM_RVR + distance_factor_b * ( LIMIT_LOWER-value ) +50;//+ direction_factor * (-direction);
			pwm_l = MIN_PWM_RVR + distance_factor_b * ( LIMIT_LOWER-value )  ;
			//pwm_r = 90;
			//pwm_l = 80;

			sendData('B', pwm_r,pwm_l);
			//printf("------%d    %d---\n",pwm_r, pwm_l);
			

		}
		else if( direction >= DIRECTION + 100 )
		{
			pwm_r = MIN_PWM_RVR + distance_factor_b * ( LIMIT_LOWER-(value+100) );
			pwm_l = MIN_PWM_RVR + distance_factor_b * ( LIMIT_LOWER-(value+100) )+50;// + direction_factor * direction ;
			
			//pwm_r = 80;
			//pwm_l = 90;

		
			sendData('B', pwm_r,pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);
		}
		else
		{
			pwm_r = MIN_PWM_RVR + 50 ;// distance_factor_b * ( LIMIT_LOWER - value ) ;
			pwm_l = MIN_PWM_RVR + 50 ;//distance_factor_b * ( LIMIT_LOWER - value ) ;
			//pwm_r = 90;
			//pwm_l = 90;

			sendData('B', pwm_r,pwm_l);
			//printf("------%d===%d-------\n",pwm_r, pwm_l);
		}


	}else if( value <= 800 )
		{
			sendData( 'S', 0, 0 );
			printf("Too Close < 80cm \n");
		}
	
	
}


bool allJointsAvailable( NUI_SKELETON_DATA* skeletonData)
{
	int jointCounter = 4;
	for( int count = 0; count < 4; count )
	{
		if( skeletonData->SkeletonPositions[count].x == 0.0 && skeletonData->SkeletonPositions[count].y == 0.0 && skeletonData->SkeletonPositions[count].z == 0.0)
			jointCounter = jointCounter + 1;
	}

	if( jointCounter == 4 )
	{
		return FALSE;
	}
	else
		return TRUE;

}
	


int main(int argc, _TCHAR* argv[])
{

	if( sp->IsConnected() )
	{
		printf(" Arduino connectivity-------> OK\n ");
	}


   printf("Initializing Kinect...");
   HRESULT hr;
   hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); //Disables the default player selection mode and enables the application to choose which detected skeletons are tracked.
   if(SUCCEEDED(hr))
   {
	   printf("> OK\n");
   }
   else
   {
	   printf("\nInirialization failed. No sensor attached! \n");
	   getchar();
   }
   NUI_SKELETON_FRAME ourframe;
   const NUI_TRANSFORM_SMOOTH_PARAMETERS parameters = {0.5f, 0.5f, 0.5f, 0.05f, 0.04f}; // good for gesture recognition
  

 //  NuiTransformSmooth( &ourframe, parameters);
   HANDLE hNextFrameEvent;
  // NuiSkeletonTrackingEnable( NULL, NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS);


   printf("<---------STARTING---------->\n");

 /*  while(1)
   {
	   hr = NuiSkeletonGetNextFrame(30, &ourframe);
	  if( SUCCEEDED( hr ))
	  {
		  printf("Image availabale\n");
	  }
	  else
	  {
		  printf("No image avaialbale\n");
	  }
   }
   */
   bool status = FALSE;
   int stepcounter = 0;
   // Main loop
   printf("Here 1\n;");
   while (1)
   {


	  // if (SUCCEEDED(NuiSkeletonGetNextFrame(30, &ourframe)))
	   while( FAILED(NuiSkeletonGetNextFrame(30, &ourframe))) 
	   {
		   // wait till successful frame obtain
	   }
	   //  printf("Here 1\n;");
			 status = FALSE;
			 stepcounter = 0;

			for (int i = 0; i < NUI_SKELETON_COUNT; i++)
			 {
	
				 if (ourframe.SkeletonData[i].dwTrackingID == 0  )
				 {
					// printf("Here 2\n;");
					 stepcounter = stepcounter +1; // track  first skeleton in the list
					 continue;
				 }// end of inner if loop
				 /* shift distance array */
		
				dis[4] = dis[3];
				dis[3] = dis[2];
				dis[2] = dis[1];
				dis[1] = dis[0];
		
				/* shift x coordinate array  */
		
				x_[4] = x_[3];
				x_[3] = x_[2];
				x_[2] = x_[1];
				x_[1] = x_[0];

		
				float hipCenterZ = ourframe.SkeletonData[i].Position.z;   // z value of center of mass
				float hipCenterX = ourframe.SkeletonData[i].Position.x;   // x value of center of mass

				printf("id %d = Center of mass  ", ourframe.SkeletonData[i].dwTrackingID);
			   printf("%f   %f", hipCenterZ ,hipCenterX );
   				printf("\n");

			   dis[0] = ( (hipCenterZ < 3.0) ? hipCenterZ : 3.0 ) * 1000.0;
				 //  x_[0] =( (abs(hipCenterX) < 0.450 )? hipCenterX : ( hipCenterX < 0 ? -0.450 : 0.450 ) )*1000.0;
			   x_[0] = hipCenterX * 1000.0;

			   if( x_[0] < -800 || x_[0] > 800 || dis[0] < 850 )//|| allJointsAvailable( &ourframe.SkeletonData[i]) == FALSE  )	   
			   {
					 printf("Out of Range \n");
					dis[0] = 0;
					dis[1] = 0;
					dis[2] = 0;
					dis[3] = 0;

					x_[0] = 0;
					x_[1] = 0;
					x_[2] = 0;
					x_[3] = 0;
				} // end of inner inner if loop
			     move_robot();    // move robot according to the gathered data
				 status = TRUE;   // already  a user is being followed		 
		 }// end of inner for loop

		if( stepcounter == NUI_SKELETON_COUNT ) // if above for loop not succeeded stop the robot. there is no skeleton availabale.
		{
			stopRobot();
			
		}// end of inner if loop
   
 }// end of while loop
  NuiShutdown();
  return 0;

	
}

