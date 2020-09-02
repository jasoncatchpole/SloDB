#include "PoseRefiner.h"

#include <iostream>
#include <fstream>

using namespace SloDB;
using namespace std;

PoseRefiner::PoseRefiner(void)
{
}

PoseRefiner::~PoseRefiner(void)
{
}

// Computes accurate poses based on a combination of the information in frames and pan_frames, the new, more accurate is then stored back into frames
bool PoseRefiner::RefineFramePoses(vector<FrameData> &frames, const vector<PanFrameData> &pan_frames)
{
	// first we better make sure this isn't one of the few files which didn't have the pan & tilt times recorded
	if( frames.size() > 0 )
	{
		if( (frames[0].panTime == -99) && (frames[0].tiltTime == -99) )
		{
			ComputeFairlyAccuratePose(frames, pan_frames);
			return true;
		}

		// otherwise its just a normal file in which case it continues below
			
	}
	else
	{
		// we have a big problem if there are no frames in the set, this function won't work so don't continue
		return false;
	}

	// go through each entry of pan_frames and use this accurate information to update certain frames from 'frames'
	vector<PanFrameData>::const_iterator pan_iter = pan_frames.begin();
	pan_iter+=2; // skip the first 2 entries as these are during initialization

	//##################################################################################//
	// Stage 1 (2 pan frames) - this stage involves moving to the left while panning right, 
	// then moving to the right while panning left
	InterpolationGeneral(frames, (*pan_iter).frameNum, 550, (*pan_iter).elapsedTime, -670, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);

	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -670, (*pan_iter).elapsedTime, 550, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);

	//##################################################################################//
	// Stage 2 (8 pan frames) - This stage involves doing a jagged (Fast) left right pan
	//							with no movement position wise
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 550, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200);


	//##################################################################################//
	// Stage 3 (6 pan frames) - This stage involves doing a up down tilt with no movement
	//							position wise
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -200, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0);
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0);

	// at this stage the up down tilt is really complete, now we are moving into position to do the Circular Motion,
	// i.e the one with the diamond shape
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -200, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, 400, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, -200); // this is a pan

	//##################################################################################//
	// Stage 4 (6 pan frames) - This stage performs a diagonal motion which involves both 
	// a pan and tilt at the same time. I need a special method to deal with this as there
	// isn't any pan and tilt measurements for this (couldn't capture them for the command
	// type EXECUTE_UPON_IMMEDIATE_OR_AWAIT which was required for doing simultaneous pan 
	// and tilt movements.
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, 400, -200, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -100, 300);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -100, 300, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -600, -200);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -600, -200, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -100, -700);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -100, -700, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, 400, 0);

	// at this stage we have technically finished the diagonal movements now we are just setting
	// up for the next stage
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 400); // this is a tilt
	pan_iter++;
	// this is the first pan after the diagonal move, there may be residual 0s in the pan values so make sure we don't have them
	if( frames[(*pan_iter).frameNum].pan == 0 ) // this shouldn't be zero it should be 400 (or something else other than 0)
		frames[(*pan_iter).frameNum].pan = 400;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 400, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 150); // this is a pan

	//##################################################################################//
	// Stage 5 (2 pan frames) - Moving Up/Down Tilt
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -550, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -550, (*pan_iter).elapsedTime, 200, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt


	//##################################################################################//
	// Stage 6 (4 pan frames) - The Up/Down while moving forward
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 200, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, 200, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt
	pan_iter++;
	InterpolationGeneral(frames, (*pan_iter).frameNum, 200, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt, 0); // this is a tilt
	pan_iter++;
	// this next movement can be skipped since it goes from -400 tilt to -400 tilt, for some reason it was the exact same movement
	//InterpolationGeneral(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*pan_iter).isTilt); // this is a tilt


	//##################################################################################//
	// Stage 7 (1 pan frame) - 
	pan_iter++;
    // TODO: pan_iter+1 is not valid. At this point we have reached the end of the pan data
	InterpolationGeneral(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, 70, frames.size(), (*pan_iter).isTilt, 0); // this is a tilt

	// first do the pan frame since that is the beginning of the movement //
	//int lastFrame = BeginpanFrame(frames, *pan_iter);
	/*double vel = CalculateAverageVelocity(frames, (*pan_iter).frameNum, (*pan_iter).elapsedTime, (*frames)[(*pan_iter).frameNum].pan, -670);
	(*frames)[(*pan_iter).frameNum].panTime = (*pan_iter).elapsedTime;	// the first frame in the sequences should be set to the accuracy of the pan_data as that is the most accurate
	int lastFrame = GeneralInterpolation(frames, (*pan_iter).frameNum, (*pan_iter).elapsedTime, NULL, (*pan_iter).isTilt);

	// now we need to do the frames in between using just the time stamps taken in between frames from the wait_completion function
	while( (*frames)[lastFrame].pan != 670 )
		lastFrame = GeneralInterpolation(frames, lastFrame, (*frames)[lastFrame].panTime, NULL, (*pan_iter).isTilt);
	//BetweenNormalFrames(frames, lastFrame);*/

	/*
	// go through each panFrame and update the frames where these pan_frames cover (as they are more accurate)
	int beginFrame = (*pan_iter).frameNum;
	int endFrame;
	double beginTime = (*pan_iter).elapsedTime;
	double endTime = 0.0;

	// ensure the orientation of the frame at the beginning is equal to the start orientation, it should be since the csv wait_completion
	// records are taken after the pan_data frames
	int oriBegin = 0, oriEnd;
	if( (*pan_iter).isTilt )
		oriBegin = (*frames)[beginFrame].Tilt;
	else
		oriBegin = (*frames)[beginFrame].pan;

	// now determine the next frame where there is an actual pan/tilt measurement taken //
	vector<FrameData>::const_iterator iter = frames->begin() + beginFrame;
	for(; iter!= frames->end(); iter++)
	{
		if( (*iter).pan != oriBegin )
		{
			endFrame = (*iter).frameNum;
			endTime = (*iter).elapsedTime;
			oriEnd = (*iter).pan;
			break;	// now that we've got the first frame that is different break outa the loop
		}
	}

	// now calculate the time difference and the difference in the pose //
	double timeDiff = endTime - beginTime;
	int posDiff = oriEnd - oriBegin;
	int frameDiff = endFrame - beginFrame;

	// now update the orientations of the frames between the beginning and end //
	for(int i=1; i<frameDiff; i++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		double curTimeDiff = (*frames)[beginFrame+i].elapsedTime - beginTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		(*frames)[beginFrame+i].pan = (perDiff * posDiff) + oriBegin;
	}

	double newpan = (*frames)[beginFrame+1].pan;*/

	return true;
}

bool PoseRefiner::ComputeFairlyAccuratePose(vector<FrameData> &frames, const vector<PanFrameData> &pan_frames)
{
	// go through each entry of pan_frames and use this accurate information to update certain frames from 'frames'
	vector<PanFrameData>::const_iterator pan_iter = pan_frames.begin();

	//##################################################################################//
	// Stage 0 - Initialization
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -200, (*pan_iter).elapsedTime, -200, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 65, 550); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 550, (*pan_iter).elapsedTime, 550, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 65, -200); // this is a pan

	//pan_iter+=2; // skip the first 2 entries as these are during initialization

	//##################################################################################//
	// Stage 1 (2 pan frames) - this stage involves moving to the left while panning right, 
	// then moving to the right while panning left
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 550, (*pan_iter).elapsedTime, -670, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 65, -200);

	//double temp = frames[104].pan;

	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -670, (*pan_iter).elapsedTime, 550, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 65, -200);

	//##################################################################################//
	// Stage 2 (8 pan frames) - This stage involves doing a jagged (Fast) left right pan
	//							with no movement position wise
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 550, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 380, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 380, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 300, -200);

    

	//##################################################################################//
	// Stage 3 (6 pan frames) - This stage involves doing a up down tilt with no movement
	//							position wise
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -200, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 0);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 0);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -580, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 0);
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -580, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 0);

	// at this stage the up down tilt is really complete, now we are moving into position to do the Circular Motion,
	// i.e the one with the diamond shape
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -200, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 0); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, 400, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, -200); // this is a pan

    OutputFramesToFile(frames, "C:\\temp\\stage12.csv");

	//##################################################################################//
	// Stage 4 (6 pan frames) - This stage performs a diagonal motion which involves both 
	// a pan and tilt at the same time. I need a special method to deal with this as there
	// isn't any pan and tilt measurements for this (couldn't capture them for the command
	// type EXECUTE_UPON_IMMEDIATE_OR_AWAIT which was required for doing simultaneous pan 
	// and tilt movements.
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, 400, -200, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -100, 300);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -100, 300, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -600, -200);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -600, -200, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, -100, -700);
	pan_iter++;
	InterpolationDiagonal(frames, (*pan_iter).frameNum, (*(pan_iter+1)).frameNum, -100, -700, (*pan_iter).elapsedTime, (*(pan_iter+1)).elapsedTime, 400, 0);

	// at this stage we have technically finished the diagonal movements now we are just setting
	// up for the next stage
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 0, (*pan_iter).elapsedTime, 150, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 45, 400); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 400, (*pan_iter).elapsedTime, 0, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 100, 150); // this is a pan


	//##################################################################################//
	// Stage 5 (2 pan frames) - Moving up down tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 150, (*pan_iter).elapsedTime, -550, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 45, 0); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -550, (*pan_iter).elapsedTime, 200, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 45, 0); // this is a tilt


	//##################################################################################//
	// Stage 6 (4 pan frames) - The Up/Down while moving forward
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 200, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 75, 0); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, 200, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 39, 0); // this is a tilt
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, 200, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 39, 0); // this is a tilt
	pan_iter++;
	// this next movement can be skipped since it goes from -400 tilt to -400 tilt, for some reason it was the exact same movement
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, -400, (*(pan_iter+1)).frameNum, (*(pan_iter+1)).elapsedTime, (*pan_iter).isTilt, 80, 0); // this is a tilt

	//##################################################################################//
	// Stage 7 (1 pan frame) - This is the final pan frame so i need to pass info on the last frame rather than the next pan_frame (since there isn't one)
	pan_iter++;
	FairlyGeneralInterpolation(frames, (*pan_iter).frameNum, -400, (*pan_iter).elapsedTime, 70, (*(frames.end()-1)).frameNum, (*(frames.end()-1)).elapsedTime, (*pan_iter).isTilt, 80, 0); // this is a tilt

	return true;
}

int PoseRefiner::InterpolationGeneral(vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, double endPos, int lastFrame, bool isTilt, double otherVal)
{
	if( beginFrame > (int)frames.size() )
	{
		// for some reason the frame information that we got from the pan_Frame is not in range of the current frame set
		return 0;
	}

	// for the first measurement we should interpolate between the information from the pan frame up to the first measurement recorded in panTime
	double startTime = beginTime;
	double endTime = 0.0;
	double startVal = beginVal;
	double endVal = 0;

	int endFrame = 0;

	// now we need to find the first measurement so we can fill in the endTime and endVal values so we can interpolate between these values
	vector<FrameData>::iterator iter = frames.begin() + beginFrame;
	while(iter <= frames.end())
	{
		for(; iter!= frames.end(); iter++)
		{
			if( isTilt )
			{
				if( ((*iter).tilt != startVal) || ((*iter).frameNum == lastFrame) )
				{
					endFrame = (*iter).frameNum;
					endTime = (*iter).tiltTime;	// normally the panTime and the Elapsed time are the same but sometimes panTime is more accurate
					endVal = (*iter).tilt;
					break;	// now that we've got the first frame that is different break outa the loop
				}
			}
			else
			{
				if( ((*iter).pan != startVal) || ((*iter).frameNum == lastFrame) )
				{
					if( (*iter).panTime == 0 ) // this means we are either approaching the circular movement or have just finised it but the values have not caught up yet
					{
						endFrame = lastFrame;
						endTime = (*iter).elapsedTime;
						endVal = endPos;
						if( (*iter).frameNum != lastFrame ) // this means we reached the circular motion point but there are frames in between which are 0
						{
							vector<FrameData>::iterator tilt_iter = iter;
							for(; tilt_iter!=frames.end(); tilt_iter++)
							{
								if( (*tilt_iter).frameNum == lastFrame )
									break;
								(*tilt_iter).tilt = otherVal;
							}
						}
					}
					else
					{
						endFrame = (*iter).frameNum;
						endTime = (*iter).panTime;	// normally the panTime and the Elapsed time are the same but sometimes panTime is more accurate
						endVal = (*iter).pan;
					}
					break;	// now that we've got the first frame that is different break outa the loop
				}
			}
		}

		// now that we have found the location of the frames to interpolate between do the interpolation
		// CALL A FUNCTION HERE WHICH INTERPOLATES BETWEEN 2 GIVEN FRAMES/TIMES AND POSITIONS
		InterframeInterpolate(frames, beginFrame, endFrame, startTime, endTime, startVal, endVal, isTilt, otherVal);

		// now set the start as the end of the last interpolation and do the next segment
		startTime = endTime;
		startVal = endVal;
		beginFrame = endFrame;

		// check to see if we have reached the end of this run or not
		if( (startVal == endPos) || (beginFrame == lastFrame) )
		{
			// we will stop now but first check to make sure the very last measurement doesn't need correcting
			//if( (*iter).elapsedTime < (*iter).panTime)
			/*if(beginFrame == lastFrame)
			{
				int temp = 5;
				temp+=5;
			}*/

			break;
		}
	}

	return 1;
}

// given the start and end frames (and hence the values), calculate all the values for the frames in between
void PoseRefiner::InterframeInterpolate(vector<FrameData> &frames, int startFrame, int endFrame, double startTime, double endTime, double beginVal, double endVal, bool isTilt, double otherVal)
{
	// now calculate the time difference and the difference in the pose //
	double timeDiff = endTime - startTime;

	vector<FrameData>::iterator start_iter = frames.begin() + startFrame;
	vector<FrameData>::iterator end_iter = frames.begin() + endFrame;

	double startVal, posDiff;
	startVal = beginVal;
	posDiff = endVal - startVal;
	/*if( isTilt )
	{
		startVal = (*start_iter).Tilt;
		posDiff = (*end_iter).tilt - startVal;
	}
	else
	{
		startVal = (*start_iter).pan;
		posDiff = (*end_iter).pan - startVal;
	}*/
	
	double result = 0;
	int curFrame = 0;

	for(; start_iter != end_iter; start_iter++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		double curTimeDiff = (*start_iter).elapsedTime - startTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		if( isTilt )
		{
			(*start_iter).tilt = (perDiff * posDiff) + startVal;
			(*start_iter).tiltTime = (*start_iter).elapsedTime;
			if((*start_iter).pan != otherVal)
				(*start_iter).pan = otherVal;
		}
		else
		{
			(*start_iter).pan = (perDiff * posDiff) + startVal;
			(*start_iter).panTime = (*start_iter).elapsedTime;
			if((*start_iter).tilt != otherVal)
				(*start_iter).tilt = otherVal;
		}

		result = (perDiff * posDiff) + startVal;
		curFrame = (*start_iter).frameNum;
	}
}

// This function is the equivalent of InterpolationGeneral however it works for those files which do not have pan times recorded
int PoseRefiner::FairlyGeneralInterpolation(vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, double endPos, int lastFrame, double endTime, bool isTilt, double speed, double otherVal)
{
	double SpeedVal = 0;
	if( endPos == beginVal )
	{
		SpeedVal = 0;
		speed = 0;
	}
	else
	{
		SpeedVal = (double)((double)abs((double)endPos - (double)beginVal)) / ((double)abs((double)endTime - (double)beginTime));

		// first make sure the velocities are going in the right direction
		if( endPos < beginVal )
		{
			if( speed > 0 )
				speed = -speed;
			if( SpeedVal > 0 )
				SpeedVal = -SpeedVal;
		}
		else
		{
			if( speed < 0 )
				speed = -speed;
			if( SpeedVal < 0 )
				SpeedVal = -SpeedVal;
		}

		// if the calculated value for the velocity based on the start and end time is within 5% of the actual value use
		// the calculated value instead as it is likely to be more accurate, if its outside that range its likely that this movement
		// involves the PTU waiting for the XY table hence there is a long time between PAN frames and thus a calculated slow
		// velocity. Conversely if there is a fast calculated speed then i should probably use that
		if( abs( SpeedVal - ( 0.95 * speed ) ) > abs( 0.05 * speed  ) )
			SpeedVal = speed;
	}

	vector<FrameData>::iterator start_iter = frames.begin() + beginFrame;
	vector<FrameData>::iterator end_iter = frames.begin() + lastFrame;

	double calculatedVal = 0;
	int curFrame = 0;

	for(; start_iter != end_iter; start_iter++)
	{
		curFrame = (*start_iter).frameNum;
		calculatedVal = (SpeedVal * ( (*start_iter).elapsedTime - beginTime )) + beginVal;

		// make sure the calculatedVal hasn't gone past the endPos
		if( !IsBetween(calculatedVal, beginVal, endPos) )
			calculatedVal = endPos;

		if( isTilt )
		{
			(*start_iter).tilt = calculatedVal;
			(*start_iter).pan = otherVal;
		}
		else
		{
			(*start_iter).pan = calculatedVal;
			(*start_iter).tilt = otherVal;
		}
	}

	return 0;
}

// Does the interpolation between frames for the diagonal movements where there is no time stamps for pan and tilt data
int PoseRefiner::InterpolationDiagonal(vector<FrameData> &frames, int beginFrame, int endFrame, double beginValpan, double beginValTilt, double beginTime, double endTime, double endPospan, double endPosTilt)
{
	double panSpeed = 100, TiltSpeed = 100;
	// the pan and tilt speed were exactly the same for this sequence so as long as the distance is the same it will finish at the same time
	if ( abs(endPospan - beginValpan) == abs(endPosTilt - beginValTilt) )
	{
		panSpeed = TiltSpeed = (endPospan - beginValpan) / (endTime - beginTime);
	}
	else // they aren't the same so the one which has to travel the furtherest will take the longest
	{
		if( abs(endPospan - beginValpan) > abs(endPosTilt - beginValTilt) )
			panSpeed = (endPospan - beginValpan) / (endTime - beginTime);
		else
			TiltSpeed = (endPosTilt - beginValTilt) / (endTime - beginTime);
	}
	//double avgVelocity = (endPospan - beginValpan) / (endTime - beginTime);

	if( endPospan < beginValpan )
	{
		// make sure the speed is negative
		if( panSpeed > 0)
			panSpeed = -panSpeed;
	}
	else // we have increasing values so make sure the speed is positive
	{
		if( panSpeed < 0)
			panSpeed = -panSpeed;
	}
	if( endPosTilt < beginValTilt )
	{
		// make sure the speed is negative
		if( TiltSpeed > 0 )
			TiltSpeed = -TiltSpeed;
	}
	else // we have increasing values so make sure the speed is positive
	{
		if( TiltSpeed < 0 )
			TiltSpeed = -TiltSpeed;
	}

	/*double timeDiff = endTime - beginTime;
	double panDiff = endPospan - beginValpan;
	double TiltDiff = endPostilt - beginValTilt;*/

	vector<FrameData>::iterator start_iter = frames.begin() + beginFrame;
	vector<FrameData>::iterator end_iter = frames.begin() + endFrame;

	double resultpan = 0, resulttilt = 0;
	int curFrame = 0;

	for(; start_iter != end_iter; start_iter++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		/*double curTimeDiff = (*start_iter).elapsedTime - beginTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		
		(*start_iter).tilt = (perDiff * TiltDiff) + beginValTilt;
		(*start_iter).tiltTime = (*start_iter).elapsedTime;
		(*start_iter).pan = (perDiff * panDiff) + beginValpan;
		(*start_iter).panTime = (*start_iter).elapsedTime;
	
		resultpan = (perDiff * panDiff) + beginValpan;
		curFrame = (*start_iter).frameNum;*/

		curFrame = (*start_iter).frameNum;
		(*start_iter).pan = panSpeed * ( (*start_iter).elapsedTime - beginTime ) + beginValpan;
		if( !IsBetween((*start_iter).pan, beginValpan, endPospan) )
		{
			if( (*start_iter).elapsedTime < beginTime ) // big problem, well kinda big, actually not big at all
			{
				cerr << "Error: The beginning of this diagonal movement is before the first elapsed time" << endl;
				(*start_iter).pan = beginValpan;
			}
			else
				(*start_iter).pan = endPospan;
		}

		(*start_iter).tilt = TiltSpeed * ( (*start_iter).elapsedTime - beginTime ) + beginValTilt;
		if( !IsBetween((*start_iter).tilt, beginValTilt, endPosTilt) )
		{
			if( (*start_iter).elapsedTime < beginTime ) // big problem, well kinda big, actually not big at all
			{
                cerr << "Error: The beginning of this diagonal movement is before the first elapsed time" << endl;
				(*start_iter).tilt = beginValTilt;
			}
			else
				(*start_iter).tilt = endPosTilt;
		}
	}
		
	return 0;
}

bool PoseRefiner::IsBetween(double curVal, double startVal, double endVal)
{
	if( endVal < startVal ) // this means we have a negative velocity
	{
		if( (curVal > startVal) || (curVal < endVal) )
			return false;
	}
	else // the start is less than the end
	{
		if( (curVal < startVal) || (curVal > endVal) )
			return false;
	}

	/*if( startVal < endVal )
	{
		if( curVal < startVal )
			return false;
		else if( curVal > endVal )
				return false;
	}
	else
	{
		if( curVal < startVal )
			return false;
		else if( curVal > endVal )
			return false;
	}*/

	return true;
}

// Computes the frames in between the first pan_data frame and the first normal frame measured
int PoseRefiner::BeginPanFrame(vector<FrameData> &frames, PanFrameData pan_frame)
{
	int beginFrame = pan_frame.frameNum;
	int endFrame;
	double beginTime = pan_frame.elapsedTime;
	double endTime = 0.0;

	// ensure the orientation of the frame at the beginning is equal to the start orientation, it should be since the csv wait_completion
	// records are taken after the pan_data frames
	double oriBegin = 0, oriEnd;
	if( pan_frame.isTilt )
		oriBegin = frames[beginFrame].tilt;
	else
		oriBegin = frames[beginFrame].pan;

	// now determine the next frame where there is an actual pan/tilt measurement taken //
	vector<FrameData>::const_iterator iter = frames.begin() + beginFrame;
	for(; iter!= frames.end(); iter++)
	{
		if( (*iter).pan != oriBegin )
		{
			endFrame = (*iter).frameNum;
			endTime = (*iter).panTime;	// normally the panTime and the Elapsed time are the same but sometimes panTime is more accurate
			oriEnd = (*iter).pan;
			break;	// now that we've got the first frame that is different break outa the loop
		}
	}

	// now calculate the time difference and the difference in the pose //
	double timeDiff = endTime - beginTime;
	double posDiff = oriEnd - oriBegin;
	int frameDiff = endFrame - beginFrame;

	// now update the orientations of the frames between the beginning and end //
	for(int i=1; i<frameDiff; i++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		double curTimeDiff = frames[beginFrame+i].elapsedTime - beginTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		frames[beginFrame+i].pan = (perDiff * posDiff) + oriBegin;
	}

	return endFrame;
}

// Computes the frames between 2 normal frames
bool PoseRefiner::BetweenNormalFrames(vector<FrameData> &frames, int beginFrame)
{
	int endFrame;
	double beginTime = frames[beginFrame].panTime;
	double endTime = 0.0;

	double oriBegin = frames[beginFrame].pan, oriEnd;

	// now determine the next frame that is different //
	vector<FrameData>::const_iterator iter = frames.begin() + beginFrame;
	for(; iter!= frames.end(); iter++)
	{
		if( (*iter).pan != oriBegin )
		{
			endFrame = (*iter).frameNum;
			endTime = (*iter).panTime;
			oriEnd = (*iter).pan;
			break;	// now that we've got the first frame that is different break outa the loop
		}
	}

	// now calculate the time difference and the difference in the pose //
	double timeDiff = endTime - beginTime;
	double posDiff = oriEnd - oriBegin;
	int frameDiff = endFrame - beginFrame;

	// now update the orientations of the frames between the beginning and end //
	for(int i=1; i<frameDiff; i++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		double curTimeDiff = frames[beginFrame+i].elapsedTime - beginTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		frames[beginFrame+i].pan = (perDiff * posDiff) + oriBegin;
	}

	return false;
}

int PoseRefiner::GeneralInterpolation(vector<FrameData> &frames, int beginFrame, double beginTime, double *StoppingPos, bool isTilt)
{
	//int beginFrame = pan_frame.frameNum;
	int endFrame;
	//double beginTime = pan_frame.elapsedTime;
	double endTime = 0.0;

	// ensure the orientation of the frame at the beginning is equal to the start orientation, it should be since the csv wait_completion
	// records are taken after the pan_data frames
	double oriBegin = 0, oriEnd;
	if( isTilt )
		oriBegin = frames[beginFrame].tilt;
	else
		oriBegin = frames[beginFrame].pan;

	// now determine the next frame where there is an actual pan/tilt measurement taken //
	vector<FrameData>::const_iterator iter = frames.begin() + beginFrame;
	for(; iter!= frames.end(); iter++)
	{
		if( StoppingPos == NULL ) // this means we are using a pan_frame so stop when the pos first changes
		{
			if( (*iter).pan != oriBegin )
			{
				endFrame = (*iter).frameNum;
				endTime = (*iter).panTime;	// normally the panTime and the Elapsed time are the same but sometimes panTime is more accurate
				oriEnd = (*iter).pan;
				break;	// now that we've got the first frame that is different break outa the loop
			}
		}
		else // if we do reach the StoppingPos we are doing intermediate frames so we need to stop completely now, reaching StoppingPos means the PTU reached its destimation
		{

		}
	}

	// now calculate the time difference and the difference in the pose //
	double timeDiff = endTime - beginTime;
	double posDiff = oriEnd - oriBegin;
	int frameDiff = endFrame - beginFrame;

	double temp = 0;

	// now update the orientations of the frames between the beginning and end //
	for(int i=1; i<frameDiff; i++)
	{
		// calculate the time difference between the frame we are trying to calculate and the first frame
		double curTimeDiff = frames[beginFrame+i].elapsedTime - beginTime;
		// now find the % of the whole time between the actual measurements so we can find the value for this frame
		double perDiff = curTimeDiff/timeDiff;
		frames[beginFrame+i].pan = (perDiff * posDiff) + oriBegin;
		frames[beginFrame+i].panTime = frames[beginFrame+i].elapsedTime;
		temp = frames[beginFrame+i].pan;
	}

	if( frames[beginFrame].elapsedTime > beginTime ) // this means the position measurement was recorded sometime after the first orientation measurement so it needs correcting
	{
		double curTimeDiff = frames[beginFrame].elapsedTime - beginTime;
		double perDiff = curTimeDiff/timeDiff;
		frames[beginFrame].pan = (perDiff * posDiff) + oriBegin;
		frames[beginFrame].panTime = frames[beginFrame].elapsedTime;
		temp = frames[beginFrame].pan;
	}

	return endFrame;
}

int PoseRefiner::Interpolation(vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, int endPos, double speed)
{
	//int beginVal = 550;
	int lastFrame = 0;
	
	vector<FrameData>::iterator iter = frames.begin() + beginFrame;
	double previousVal = (*iter).pan;
	for(; iter!= frames.end(); iter++)
	{
		// interpolate the orientation for this frame //
		double newOri = speed * ( (*iter).elapsedTime - beginTime ) + beginVal;

		if( (*iter).pan == endPos )
		{
			lastFrame = (*iter).frameNum;
			break;
		}

		// we should ensure that our speed value really does correspond with the values that were recorded at the time of capture
		if( (*iter).pan != previousVal ) // the value has changed so we have an updated time stamp too
		{
			// lets update the timer and the begin value to provide closer estimates (i.e more local estimates)
			beginVal = (*iter).pan;
			beginTime = (*iter).panTime;

			double oriCheck = speed * ( (*iter).panTime - beginTime ) + beginVal;
			if(! (((oriCheck/(*iter).pan) > 0.95) && ((oriCheck/(*iter).pan) < 1.05) )) // its close enough
			{
				//::MessageBox(	TEXT("The estimate based on the speed is not close enough to actual measurements.\r\n\r\n"),
				//			TEXT("Invalid File"), MB_OK | MB_ICONINFORMATION);
                cerr << "Error: Estimate based on speed is not close enough to actual measurement" << endl;
			}

			previousVal = (*iter).pan;
		}

		(*iter).pan = newOri;
		(*iter).panTime = (*iter).elapsedTime;
	}

	return lastFrame;
}

double PoseRefiner::CalculateAverageVelocity(vector<FrameData> &frames, int beginFrame, double beginTime, double beginPos, double endPos)
{
	// first we need to determine the final frame, and final time
	int lastFrame = 0;

	// go through each frame looking for the first frame to have the endPos value
	vector<FrameData>::const_iterator iter = frames.begin() + beginFrame;
	for(; iter!= frames.end(); iter++)
	{
		if( (*iter).pan == endPos )
		{
			lastFrame = (*iter).frameNum;
			break;
		}
	}

	// now that we've found that, backtrack a little to the next frame which had a time recording taken as that is more accurate
	iter--; // we want the first instance of the value before the endPos value
	double valBefore = (*iter).pan;
	// now keep backtracking till we find the first instance of the valBefore value
	while( (*(--iter)).pan == valBefore )
	{

	}

	iter++; // now we are pointing at the most recent backtracked frame with a time record
	double endTime = (*iter).panTime;
	double backEndPos = (*iter).pan;
	int backFrame = (*iter).frameNum;

	// using the position of this backtracked frame and the time it was taken calculate the average velocity
	double avgVelocity = (beginPos - backEndPos) / (endTime - beginTime);

	// now using interpolation (with the calculated average) make sure the last frame that we found first (the one with the value 'endPos')
	// really would have the value 'endPos' based on the time the positional measurement was taken

	return avgVelocity;
}

void PoseRefiner::OutputFramesToFile(vector<FrameData> &frames, const string &outputFilename)
{
	// try opening the file
	//CString full_name = source->GetFilename();

	// locate the '.' which indicates the file extension, this should support any type of movie file
	//int ext_index = full_name.ReverseFind('.');
	//CString name = full_name.Right(full_name.GetLength() - ext_index);
		
	//full_name.Replace(name, "_FINAL.csv");

	// outputs the contents of the frame_data vector to a CSV file //
	ofstream fileout(outputFilename, ios::out | ios::trunc ); //ios::out | ys

	if(!fileout)
	{
		cout << __FILE__ << " could not open the file for outputting the frame data"<< endl;
		return;
	}

	//vector< FrameData> *frames = source->GetFrames();
	vector< FrameData>::iterator iter = frames.begin();

	for(; iter != frames.end(); iter++)
	{
		fileout << (*iter).frameNum << "," << (*iter).xPos << "," << (*iter).yPos << ",0,0,0," << (*iter).elapsedTime << "," << (*iter).stage << "," << (*iter).pan << "," << (*iter).panTime << "," << (*iter).tilt << "," << (*iter).tiltTime << "\n";
	}

	fileout.close();
}