#pragma once

#include <vector>
#include <map>
#include "FrameData.h"

namespace SloDB
{
    class FrameMatcher
    {
    public:
        FrameMatcher(const std::vector< std::vector<FrameData>> &allFrames);
        ~FrameMatcher();

        // A seek has occured in the main video so find the positions the rest of the videos should be in to stay in sync with this one
        int Seek(int mainPos, std::vector< int > &seekPositions);
    private:
        // this is the vector of frame sets for each of the videos which we are trying to sync, the video in the first position of the vector
        // is the main video to which we are trying to sync the remaining videos
        std::vector< std::vector<FrameData> > videoFrames;

        // this stores the frame number of the first and last frame in a given stage for each video
        std::vector< std::vector< std::pair< int, int> > > stageList;

        int mainIndex = 0;

        // Calculates the distance between 2 frames
        double Distance(FrameData frame1, FrameData frame2);

        // Searches through 'frames' to find the closest frame in pose to 'mainFrame'
        int LocateClosestFrame(FrameData mainFrame, int frameSetIndex);
        
        // Stores the locations of the starts and ends of each stage for each video file so that they can be searched more quickly
        void BuildStageList(void);
        
        // Locates the start and end frame for a given stage in a set of frames
        void FindStageStartEnd(int stage, const std::vector<FrameData> &frames, int & start, int & end, int offset);
    };
}
