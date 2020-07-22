#include "FrameMatcher.h"

#include <math.h>
#include <iostream>

using namespace std;

namespace SloDB
{
    FrameMatcher::FrameMatcher(const vector< vector<FrameData>> &allFrames)
    {
        videoFrames = allFrames;

        // lets process the VideoFrames a little to improve indexing
        BuildStageList();
    }

    FrameMatcher::~FrameMatcher(void)
    {
        if (!videoFrames.empty())
            videoFrames.clear();

        if (!stageList.empty())
            stageList.clear();
    }

    // Stores the locations of the starts and ends of each stage for each video file so that they can be searched more quickly
    void FrameMatcher::BuildStageList(void)
    {
        for (int i = 0; i <= 8; i++) // go through each of the 8 stages
        {

            vector< pair< int, int> > curStage;

            for (unsigned int j = 0; j < videoFrames.size(); j++)
            {
                pair<int, int> curVid;

                int start = 0, end = 0;

                FindStageStartEnd(i, videoFrames[j], start, end, 0);

                curVid.first = start;
                curVid.second = end;

                curStage.push_back(curVid);
            }

            stageList.push_back(curStage);

        }
    }

    // A seek has occured in the main video so find the positions the rest of the videos should be in to stay in sync with this one
    int FrameMatcher::Seek(int mainPos, vector< int > &seekPositions)
    {
        // make sure the number of entires (videos) in VideoFrames is >= 2
        if (videoFrames.size() < 2)
        {
            cerr << "Trying to seek and sync when there aren't even enough videos to sync" << endl;
            return 0;
        }
        if (mainPos > videoFrames[0].size())
        {
            cerr << "Trying to seek to a location in the main video which is out of range of the video (the video doesn't have this many frames)" << endl;
            return 0;
        }

        // a seek has occurred, where mainPos indicates the position we are seeking to in the main video which is in position 0 of VideoFrames

        // go through each combination of main frame with each of the remaining videos finding the closest frame to the main one //
        FrameData mainFrame = videoFrames[0][mainPos];

        for (unsigned int i = 1; i < videoFrames.size(); i++)
        {
            int index = LocateClosestFrame(mainFrame, i);

            seekPositions.push_back(index);
        }

        return 0;
    }

    // Calculates the distance between 2 frames
    double FrameMatcher::Distance(FrameData frame1, FrameData frame2)
    {
        double dist = 0.0;

        // calculate the euclidean distance between the two positions and orientations //
        double distOri = pow(frame1.pan - frame2.pan, 2) + pow(frame1.tilt - frame2.tilt, 2);
        distOri = sqrt(distOri);

        double distPos = pow(frame1.xPos - frame2.xPos, 2) + pow(frame1.yPos - frame2.yPos, 2);
        distPos = sqrt(distPos);

        dist = distOri + (0.1*distPos); // position has less of an impact than orientation, also position has a larger range therefore scale the effect of position difference

        return dist;
    }

    // Searches through 'frames' to find the closest frame in pose to 'mainFrame'
    int FrameMatcher::LocateClosestFrame(FrameData mainFrame, int frameSetIndex)
    {
        int matchIndex = -1;
        double closestDistance = 999999999;

        // first get the rough position by using the stage of the main frame and getting the corresponding stage in the other videos
        int beginFrame = stageList[mainFrame.stage][frameSetIndex].first;
        int endFrame = stageList[mainFrame.stage][frameSetIndex].second;

        vector<FrameData>::iterator iter = videoFrames[frameSetIndex].begin() + beginFrame;
        vector<FrameData>::iterator end_iter = videoFrames[frameSetIndex].begin() + endFrame;

        for (; iter != end_iter; iter++)
        {
            double curDistance = Distance(mainFrame, (*iter));

            if (curDistance < closestDistance)
            {
                closestDistance = curDistance;
                matchIndex = (*iter).frameNum;
            }
        }

        if (closestDistance > 10)
            matchIndex = -1;

        return matchIndex; // returns the index of the closest frame in 'frames' to 'mainFrame', returns -1 if no match found
    }

    // Locates the start and end frame for a given stage in a set of frames
    void FrameMatcher::FindStageStartEnd(int stage, const vector<FrameData> &frames, int & start, int & end, int offset)
    {
        start = end = 0;

        vector<FrameData>::const_iterator iter = frames.begin() + offset;

        // first find the start of the stage
        for (; iter != frames.end(); iter++)
        {
            if ((*iter).stage == stage)
            {
                start = (*iter).frameNum;
                break;
            }
        }

        // now find the end
        for (; iter != frames.end(); iter++)
        {
            if ((*iter).stage != stage)
            {
                end = (*iter).frameNum - 1;
                break;
            }
        }

        if (end == 0) // this means we were searching for stage number 8 which would hit the end before it could set the end frame value
            end = frames.size() - 1;

    }
}