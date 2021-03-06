// slodb_cpp: This console application takes in one or more videos from the SloDB dataset
// and makes demonstrates the use of the ground truth data to find the closest matching frames
// between a source video and many other videos. It can dump out corresponding frames to a csv
// file

#include <fstream>
#include <filesystem>

#include "tclap/CmdLine.h"

#include "FrameData.h"
#include "PoseRefiner.h"
#include "FrameMatcher.h"

using namespace SloDB;
using namespace std;
using namespace std::experimental::filesystem;

const string sloDbMatcherSoftwareVersion = "0.10";

void MatchFrame(int frameToMatch, FrameMatcher *matcher);

int main(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("SloDB frame matcher", ' ', sloDbMatcherSoftwareVersion);
    TCLAP::ValueArg<string> referenceArg("r", "referenceVideo", "The path to the reference video", true, "", "string");
    TCLAP::UnlabeledMultiArg<string> filesArg("files", "The list of files to be matched to the reference video", true, "string");

    cmd.add(referenceArg);
    cmd.add(filesArg);

    // Parse the argv array.
    cmd.parse(argc, argv);

    // Go through and make sure all the files exist
    if (!exists(referenceArg.getValue()))
    {
        cerr << "Error: reference file does not exist. Path = " << referenceArg.getValue() << endl;
        return -1;
    }
    vector<string> fileList = filesArg.getValue();
    for (const auto & currentFile : fileList)
    {
        if (!exists(currentFile))
        {
            cerr << "Error: file specified does not exist. Path = " << currentFile << endl;
            return -1;
        }
    }
    fileList.insert(fileList.begin(), referenceArg.getValue()); // we'll stick the reference video right at the start

    // now replace the video file names with the ground truth and pandata file names
    vector<string> pandataList;
    for (auto & currentFile : fileList)
    {
        // find the .avi extension
        if (currentFile.find_last_of(".avi") == string::npos)
        {
            cerr << "Error: Cannot find the .avi extension in video file with path " << currentFile << endl;
            return -1;
        }

        cout << "Filename was " << currentFile;

        // replace .avi extension with .csv
        currentFile.replace(currentFile.length() - 4, 4, ".csv");
        string pandataFile = currentFile;
        pandataFile.replace(currentFile.length() - 4, 12, "_PANDATA.csv");
        pandataList.push_back(pandataFile);

        cout << " now is " << currentFile << endl;

        if (!exists(currentFile))
        {
            cerr << "Error: FrameData file specified does not exist. Path = " << currentFile << endl;
            return -1;
        }
        if (!exists(pandataFile))
        {
            cerr << "Error: PAN data file specified does not exist. Path = " << pandataFile << endl;
            return -1;
        }
    }

    vector<vector<FrameData>> allFiles; // this vector of vectors will have all FrameData in it. element 0 will have reference data
    //allFiles.reserve(fileList.size());
    allFiles.resize(fileList.size());

    // files exist, lets begin
    cout << "Starting SloDB matcher. Reference file is " << referenceArg.getValue() <<", using file list ";
    for(const auto & current_file : filesArg.getValue())
    {
        cout << current_file << ", ";
    }
    cout << endl;

    // Go through each file and read its contents in and then form a FrameData vector
    // iterate over all files, read them and parse their contents into a FrameData vector
    string currentLine;
    int index = 0;
    for (const auto & currentFile : fileList)
    {
        ifstream currentFileStream(currentFile);
        while (getline(currentFileStream, currentLine))
        {
            allFiles[index].push_back(FrameData(currentLine));
        }

        index++;
    }

    // next lets read in all the pandata
    vector<vector<PanFrameData>> allPandata;
    allPandata.resize(pandataList.size());
    currentLine.clear();
    index = 0;
    for (const auto & currentFile : pandataList)
    {
        ifstream currentFileStream(currentFile);
        while (getline(currentFileStream, currentLine))
        {
            allPandata[index].push_back(PanFrameData(currentLine));
        }

        index++;
    }

    // now refine each of the sets of reference frames by using the pose calculator which will combine the pandata with the frames
    PoseRefiner poseRefiner;
    for (int index = 0; index < allFiles.size(); index++)
        poseRefiner.RefineFramePoses(allFiles[index], allPandata[index]);

    // now pass all the frame data to the slider finder which is responsible for being able to sync up videos
    FrameMatcher frameMatcher(allFiles);

    MatchFrame(5, &frameMatcher);
    MatchFrame(25, &frameMatcher);
    MatchFrame(100, &frameMatcher);
    MatchFrame(200, &frameMatcher);
    MatchFrame(500, &frameMatcher);

    cout << "About to match all frames" << endl;
    // go through all frames from the reference video and produce corresponding frames in each of the other videos
    for (int frameIndex = 0; frameIndex < allFiles[0].size(); frameIndex++)
    {
        vector<int> seekPositions;
        frameMatcher.Seek(frameIndex, seekPositions);
    }

    return 0;
}

void MatchFrame(int frameToMatch, FrameMatcher *matcher)
{
    vector<int> seekPositions;
    matcher->Seek(frameToMatch, seekPositions);
    cout << "Match for frame " << frameToMatch << " is ";
    for (int index = 0; index < seekPositions.size(); index++)
    {
        cout << seekPositions[index] << ", ";
    }
    cout << endl;
}