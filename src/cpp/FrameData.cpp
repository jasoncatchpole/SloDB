#include "FrameData.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

namespace SloDB
{
    FrameData::FrameData(const string &lineFromFile)
    {
        // tokenize the string based on the ',' character
        vector<string> tokens;
        string currentToken;
        istringstream tokenizer(lineFromFile);
        while (getline(tokenizer, currentToken, ','))
        {
            tokens.push_back(currentToken);
        }

        if (tokens.size() < 8)
        {
            cerr << "Error: An insufficient number of parameters were found on the current line for this frame of data" << endl;
            return;
        }

        frameNum = stoi(tokens[0]);
        xPos = stoi(tokens[1]);
        yPos = stoi(tokens[2]);
        //curFrame.IC2_yaw	= (float)stof(Tokens[3]);
        //curFrame.IC2_pitch	= (float)stof(Tokens[4]);
        //curFrame.IC2_roll	= (float)stof(Tokens[5]);
        elapsedTime = stof(tokens[6]);
        stage = stoi(tokens[7]);

        if (tokens.size() != 12) // this might be one of the files where the data from the pan/tilt unit was not recorded
        {
            pan = 0;
            panTime = -99;
            tilt = 0;
            tiltTime = -99;
        }
        else
        {
            pan = stof(tokens[8]);
            panTime = stof(tokens[9]);
            tilt = stof(tokens[10]);
            tiltTime = stof(tokens[11]);
        }
    }


    // PANDATA
    PanFrameData::PanFrameData(const string &lineFromFile)
    {
        // tokenize the string based on the ',' character
        vector<string> tokens;
        string currentToken;
        istringstream tokenizer(lineFromFile);
        while (getline(tokenizer, currentToken, ','))
        {
            tokens.push_back(currentToken);
        }

        if (tokens.size() < 5)
        {
            cerr << "Error: An insufficient number of parameters were found on the current line for this frame of data" << endl;
            return;
        }

        frameNum = stoi(tokens[0]);
        lineNum = stoi(tokens[1]);
        elapsedTime = stof(tokens[2]);
        isTilt = stoi(tokens[3]) == 1;
        stage = stoi(tokens[4]);
    }

    // Uses the information from the time stamps to accurately determine the frames pose
    /*FrameData * FrameData::CalculatePose(FrameData PreviousFrame)
    {
        FrameData *newFrame = new FrameData();

        if( Pan == PreviousFrame.Pan ) // the pan is exactly the same here as the previous frame so no need for calculating
        {

        }
        else
        {
            //double time_diff = ElapsedTime -
        }

        return nullptr;
    }*/
}
