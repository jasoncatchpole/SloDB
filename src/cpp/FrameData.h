#pragma once

#include <string>

namespace SloDB
{
    // stores position and orientation information for every frame in the video
    class FrameData
    {
    public:
        FrameData() {};
        FrameData(const std::string &lineFromFile);
        ~FrameData() {}

    public:
        // The frame number
        int frameNum = 0;
        // The stage the program was in when this frame was captured
        int stage = 0;
        // the time that has elapsed since the program started
        double elapsedTime = 0.0f;
        // The X and Y position on the XY Table
        int xPos = 0, yPos = 0;
        // the yaw, pitch and roll from the IC2
        //float ic2Yaw, ic2Pitch, ic2Roll;

        // The following entries are only available on recordings made after 10am on the 24th
        // as that is when i started recording those values
        // The Pan position and Tilt position as measured from the Pan/Tilt Unit
        double pan = 0.0f, tilt = 0.0f;
        // the time at which the pan and tilt measurements were taken
        double panTime = 0.0f, tiltTime = 0.0f;
    };

    // stores some discrete values containing the frame numbers where each of the 8 stages kicked
    // off. Used in the PoseRefiner for interpolating pose values
    class PanFrameData
    {
    public:
        PanFrameData() {}
        PanFrameData(const std::string &lineFromFile);
        ~PanFrameData() {}
    public:
        // The frame number
        int frameNum = 0;
        // The line number in the source where this pan or tilt started
        int lineNum = 0;
        // The stage the program was in when this frame was captured
        int stage = 0;
        // the time that has elapsed since the program started
        double elapsedTime = 0.0f;
        // a 0 indicates this frame records a Pan movement a 1 indicates a Tilt
        bool isTilt = false;
    };
}
