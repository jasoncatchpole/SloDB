#pragma once

#include <vector>
#include "FrameData.h"

namespace SloDB
{
    class PoseRefiner
    {
    public:
        PoseRefiner(void);
        ~PoseRefiner(void);
        // Computes accurate poses based on a combination of the information in frames and pan_frames, the new, more accurate is then stored back into frames
        bool RefineFramePoses(std::vector<FrameData> &frames, const std::vector<PanFrameData> &pan_frames);

        void OutputFramesToFile(std::vector<FrameData> &frames, const std::string &outputFilename);
    private:
        // Computes the frames in between the first pan_data frame and the first normal frame measured
        int BeginPanFrame(std::vector<FrameData> &frames, PanFrameData pan_frame);
        // Computes the frames between 2 normal frames
        bool BetweenNormalFrames(std::vector<FrameData> &frames, int beginFrame);

        // this function is a general function which is a combination of the above two.
        // 'StoppingPos' is the value that should be stopped at once it is reached
        int GeneralInterpolation(std::vector<FrameData> &frames, int beginFrame, double beginTime, double *StoppingPos, bool IsTilt);

        int InterpolationGeneral(std::vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, double endPos, int lastFrame, bool IsTilt, double otherVal);
        int FairlyGeneralInterpolation(std::vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, double endPos, int lastFrame, double endTime, bool IsTilt, double speed, double otherVal);
        void InterframeInterpolate(std::vector<FrameData> &frames, int beginFrame, int endFrame, double startTime, double endTime, double beginVal, double endVal, bool IsTilt, double otherVal);
        int InterpolationDiagonal(std::vector<FrameData> &frames, int beginFrame, int endFrame, double beginValPan, double beginValTilt, double beginTime, double endTime, double endPosPan, double endPosTilt);

        double CalculateAverageVelocity(std::vector<FrameData> &frames, int beginFrame, double beginTime, double beginPos, double endPos);

        int Interpolation(std::vector<FrameData> &frames, int beginFrame, double beginVal, double beginTime, int endPos, double speed);

        bool IsBetween(double curVal, double startVal, double endVal);

        bool ComputeFairlyAccuratePose(std::vector<FrameData> &frames, const std::vector<PanFrameData> &pan_frames);
    };
}
