#include "SliderFinder.h"

#include <math.h>
#include <iostream>

using namespace std;

namespace SloDB
{
    SliderFinder::SliderFinder(vector< vector<FrameData> * > vid_frames)
    {
        VideoFrames = vid_frames;

        // lets process the VideoFrames a little to improve indexing
        BuildStageList();

        /*for(int i=0; i<StageList.size(); i++)
            for(int j=0; j<StageList[i].size(); j++)
            {
                int temp1 = StageList[i][j].first;
                int temp2 = StageList[i][j].second;
            }*/
    }

    /*SliderFinder::SliderFinder(std::map< int, VideoSource * > vid_frames, int mainindex)
    : MainIndex(mainindex), Vid_Frames(vid_frames)
    {
        //Vid_Frames = vid_frames;

        BuildStageMap();
    }*/

    SliderFinder::~SliderFinder(void)
    {
        if (!VideoFrames.empty())
            VideoFrames.clear();

        //if( !Vid_Frames.empty() )
        //	Vid_Frames.clear();

        if (!StageList.empty())
            StageList.clear();

        //if( !StageMap.empty() )
        //	StageMap.clear();
    }

    // Stores the locations of the starts and ends of each stage for each video file so that they can be searched more quickly
    void SliderFinder::BuildStageList(void)
    {
        //StageList = std::vector< std::vector< std::pair< int, int> > >(8);

        for (int i = 0; i <= 8; i++) // go through each of the 8 stages
        {

            vector< pair< int, int> > curStage;

            for (unsigned int j = 0; j < VideoFrames.size(); j++)
            {
                std::pair<int, int> curVid;

                int start = 0, end = 0;

                FindStageStartEnd(i, (*VideoFrames[j]), start, end, 0);

                curVid.first = start;
                curVid.second = end;

                curStage.push_back(curVid);
            }

            StageList.push_back(curStage);

        }
    }

    /*void SliderFinder::BuildStageMap(void)
    {
        //StageList = std::vector< std::vector< std::pair< int, int> > >(8);

        for(int i=0; i<=8; i++) // go through each of the 8 stages
        {

            std::map< int, std::pair< int, int> > curStage;
            std::map< int, VideoSource * >::iterator iter = Vid_Frames.begin();

            for(; iter!=Vid_Frames.end(); iter++)
            {
                std::pair<int, int> curVid;

                int start = 0, end = 0;

                FindStageStartEnd(i, (*(*iter).second->GetFrames()), start, end, 0);
                //FindStageStartEnd(i, (*VideoFrames[j]), start, end, 0);

                curVid.first = start;
                curVid.second = end;

                curStage[(*iter).first] = curVid;
            }

            StageMap.push_back(curStage);

        }
    }*/
    // A seek has occured in the main video so find the positions the rest of the videos should be in to stay in sync with this one
    int SliderFinder::Seek(int mainPos, std::vector< int > &seekPositions)
    {
        // make sure the number of entires (videos) in VideoFrames is >= 2
        if (VideoFrames.size() < 2)
        {
            cerr << "Trying to seek and sync when there aren't even enough videos to sync" << endl;
            return 0;
        }
        if (mainPos > (*VideoFrames[0]).size())
        {
            cerr << "Trying to seek to a location in the main video which is out of range of the video (the video doesn't have this many frames)" << endl;
            return 0;
        }

        // a seek has occurred, where mainPos indicates the position we are seeking to in the main video which is in position 0 of VideoFrames

        // go through each combination of main frame with each of the remaining videos finding the closest frame to the main one //
        FrameData mainFrame = (*VideoFrames[0])[mainPos];

        for (unsigned int i = 1; i < VideoFrames.size(); i++)
        {
            int index = LocateClosestFrame(mainFrame, i);

            seekPositions.push_back(index);
        }

        return 0;
    }

    // NOT SURE HOW THIS DIFFERS FROM THE 'Seek' FUNCTION, COMMENTING OUT AS IT USES VideoSource
    /*int SliderFinder::SeekFrames(int mainPos, std::map< int, int > &seekPositions)
    {
        //int mainFramePos = Vid_Frames[MainIndex]->ConvertPosToFrame(mainPos);
        // make sure the number of entires (videos) in VideoFrames is >= 2
        if( Vid_Frames.size() < 2 )
        {
            cerr << "Trying to seek and sync when there aren't even enough videos to sync" << endl;
            return 0;
        }
        if( mainPos > Vid_Frames[MainIndex]->GetFrames()->size() )
        {
            cerr << "Trying to seek to a location in the main video which is out of range of the video (the video doesn't have this many frames)" << endl;
            return 0;
        }

        // a seek has occurred, where mainPos indicates the position we are seeking to in the main video which is in position 0 of VideoFrames

        // go through each combination of main frame with each of the remaining videos finding the closest frame to the main one //
        FrameData mainFrame = (*Vid_Frames[MainIndex]->GetFrames())[mainPos];

        std::map< int, VideoSource * >::iterator iter = Vid_Frames.begin();

        for(; iter!=Vid_Frames.end(); iter++)
        {
            if( (*iter).first == MainIndex )
                continue;

            int index = LocateClosest(mainFrame, (*iter).first);

            seekPositions[(*iter).first] = index;
            //seekPositions.push_back(index);
        }

        return 1;
    }*/

    // Calculates the distance between 2 frames
    double SliderFinder::Distance(FrameData frame1, FrameData frame2)
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
    int SliderFinder::LocateClosestFrame(FrameData mainFrame, int frameSetIndex)
    {
        int matchIndex = -1;
        double closestDistance = 999999999;

        // first get the rough position by using the stage of the main frame and getting the corresponding stage in the other videos
        int beginFrame = StageList[mainFrame.stage][frameSetIndex].first;
        int endFrame = StageList[mainFrame.stage][frameSetIndex].second;

        std::vector<FrameData>::iterator iter = (*VideoFrames[frameSetIndex]).begin() + beginFrame;
        std::vector<FrameData>::iterator end_iter = (*VideoFrames[frameSetIndex]).begin() + endFrame;

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

    // THE BELOW VERSION USES THE 'StageMap' WHICH REQUIRES 'VideoSource', NOT SURE HOW IT DIFFERS FROM 'LocateClosestFrame' ABOVE
    /*int SliderFinder::LocateClosest(FrameData mainFrame, int frameSetIndex, double errorTol, double previousClosest)
    {
        int matchIndex = -1;
        double closestDistance = previousClosest;

        // first get the rough position by using the stage of the main frame and getting the corresponding stage in the other videos
        int beginFrame = StageMap[mainFrame.stage][frameSetIndex].first;
        int endFrame = StageMap[mainFrame.stage][frameSetIndex].second;
        int totalFrames = endFrame - beginFrame;

        int startI = StageMap[mainFrame.stage][MainIndex].first;
        int endI = StageMap[mainFrame.stage][MainIndex].second;
        // now determine how far through the stage the mainFrame was
        double mainPer = (double)(mainFrame.FrameNum - StageMap[mainFrame.stage][MainIndex].first) /
            (StageMap[mainFrame.stage][MainIndex].second - StageMap[mainFrame.stage][MainIndex].first);

        if( !mainFrame.stage == 0 ) // only use the updated start and end values when not in stage 0 cause in 0 all the sources have started
        // at different times
        {
        // now that we have an estimate of roughly how far the frame we are searching for appears in the stage
        // use these values to find closer estimates of the start and end frames for searching purposes, this makes
        // searching faster and reduces false matching frames
            if( mainPer < (1-errorTol) ) // if mainPer is > 0.9 then we should leave endFrame as the last frame in the stage
                endFrame = beginFrame + ((mainPer + errorTol) * totalFrames); // make endFrame 10% larger than mainPer
            if( mainPer > errorTol ) // if mainPer is < 0.1 then we have to leave the beginFrame as the first frame in the stage
                beginFrame+= (mainPer - errorTol) * totalFrames; // make the start frame within 10% of the main frames position
        }

        std::vector<FrameData>::iterator iter = (*Vid_Frames[frameSetIndex]->GetFrames()).begin() + beginFrame;
        std::vector<FrameData>::iterator end_iter = (*Vid_Frames[frameSetIndex]->GetFrames()).begin() + endFrame;

        for(; iter!= end_iter; iter++)
        {
            double curDistance = Distance(mainFrame, (*iter));

            if( curDistance < closestDistance )
            {
                closestDistance = curDistance;
                matchIndex = (*iter).FrameNum;
            }
        }

        if( closestDistance > 20 ) // perhaps we can find a closer one by relaxing the the error tolerance
            if( errorTol != 0.05 ) // make sure we weren't already doing a revised estimate
            {
                int newClosest = LocateClosest(mainFrame, frameSetIndex, 0.05, closestDistance);
                if( newClosest  != -1) // if this wasn't -1 then there was a closer one
                    matchIndex = newClosest;
            }

        return matchIndex; // returns the index of the closest frame in 'frames' to 'mainFrame', returns -1 if no match found
    }*/

    // Locates the start and end frame for a given stage in a set of frames
    void SliderFinder::FindStageStartEnd(int stage, std::vector<FrameData> frames, int & start, int & end, int offset)
    {
        start = end = 0;

        std::vector<FrameData>::iterator iter = frames.begin() + offset;

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

    // COMMENTING OUT AS IT USES StageMap and VideoSource
    // Adds another video source, extracting the start and end frames
    /*void SliderFinder::AddSource(int FrameIndex, VideoSource * source)
    {
        if( Vid_Frames[FrameIndex] )
            delete Vid_Frames[FrameIndex];
        Vid_Frames[FrameIndex] = source;

        for(int i=0; i<=8; i++) // go through each of the 8 stages
        {
            std::pair<int, int> curVid;

            int start = 0, end = 0;

            FindStageStartEnd(i, (*source->GetFrames()), start, end, 0);

            curVid.first = start;
            curVid.second = end;

            StageMap[i][FrameIndex] = curVid;
        }
    }*/
}