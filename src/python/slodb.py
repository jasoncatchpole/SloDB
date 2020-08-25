"""Entry point of application"""

import argparse
import sys
from pathlib import Path
from ground_truth_parser import GroundTruthParser

""" Links for some python GUI related functionality:

https://stackoverflow.com/questions/32342935/using-opencv-with-tkinter
https://www.pyimagesearch.com/2016/05/30/displaying-a-video-feed-with-opencv-and-tkinter/
https://www.youtube.com/watch?v=-Dp2_X9q7GU
"""

def parse_command_line() -> tuple:
    """
    Parses the command line parameters and returns them as a tuple
    """
    arg_parser = argparse.ArgumentParser(description='Command line arguments for SloDB video frame matching')
    arg_parser.add_argument('--reference_video', help='The main reference video, all other videos will be matched to this',\
        required=True, type=str)
    arg_parser.add_argument('--files', nargs='+', help='Additional videos which will be synced to the reference video', required=True)

    args = arg_parser.parse_args()
    return args.reference_video, args.files

def check_all_files_exist(file_list):
    # check to make sure all the files exist
    for current_file in file_list:
        if not Path(current_file).is_file():
            print(f'ERROR: Video cannot be found. File = {current_file}')
            sys.exit(-1)

def form_ground_truth_names(file_list) -> tuple:
    # now form file names for the ground truth and pandata file names
    ground_truth_list = []
    pandata_list = []
    for current_file in file_list:
        if not current_file.endswith(".avi"):
            print(f'ERROR: Cannot find the .avi extension in video file with path {current_file}')
            sys.exit(-1)
        
        current_ground_truth = current_file[:-4] + '.csv'
        current_pandata = current_file[:-4] + '_PANDATA.csv'

        if not Path(current_ground_truth).is_file():
            print(f'ERROR: Ground truth file does not exist. Path = {current_ground_truth}')
            sys.exit(-1)
        if not Path(current_pandata).is_file():
            print(f'ERROR: Ground truth PANDATA file does not exist. Path = {current_pandata}')
            sys.exit(-1)

        ground_truth_list.append(current_ground_truth)
        pandata_list.append(current_pandata)

    return ground_truth_list, pandata_list

def read_ground_truth_files(ground_truth_list, pandata_list) -> tuple:
     # loop over each file and read the frame data. We should have a vector of vectors
    all_files = []
    file_parser = GroundTruthParser()
    for current_file in ground_truth_list:
        file_lines = file_parser.read_file(current_file)
    
        # now lets convert these file lines into frame data structures
        frame_data = file_parser.parse_lines(file_lines)
        all_files.append(frame_data)

    all_pandata = []
    for current_file in pandata_list:
        file_lines = file_parser.read_file(current_file)
    
        # now lets convert these file lines into frame data structures
        pan_data = file_parser.parse_lines(file_lines)
        all_pandata.append(pan_data)

    return all_files, all_pandata

def main():
    # parse the command line parameters
    reference_video, file_list = parse_command_line()
    
    # make sure reference file exists
    if not Path(reference_video).is_file():
        print(f'ERROR: Reference video cannot be found. File = {reference_video}')
        sys.exit(-1)

    check_all_files_exist(file_list)

    # add the reference video to the start of the file list
    file_list = [reference_video] + file_list

    # form the ground truth filenames and ensure those files actually exist
    ground_truth_list, pandata_list = form_ground_truth_names(file_list)

    print(f'Starting SloDB matcher. Reference = {reference_video}, file list = {file_list}')

    all_files, all_pandata = read_ground_truth_files(ground_truth_list, pandata_list)
    
    
    #print(f'First file line is {file_lines[0]}')

    
    #print(f'First frame data is {frame_data[0].x_pos}')

if __name__ == "__main__":
    main()