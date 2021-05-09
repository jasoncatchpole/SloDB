"""Reads in and parses ground truth files to extract the frame data for associated with an
entire video"""

from pathlib import Path
from frame_data import FrameData, PanFrameData

class GroundTruthParser:

    def __init__(self):
        """Inentionally left blank"""
        pass
        

    def read_file(self, ground_truth_path) -> list:
        """Reads in the provided ground truth file.

        Arguments:
            ground_truth_path {str} -- The path to the ground truth file to read

        Returns:
            list -- A list of lines from the ground truth file
        """
        # make sure the file exists
        if not Path(ground_truth_path).is_file():
            print(f'ERROR: Cannot find the ground truth file with path {ground_truth_path}')

        with open(ground_truth_path, 'r') as reader:
            # read all the lines from the file into a list called "file_lines"
            file_lines = list(reader)

        return file_lines

    def parse_lines(self, file_lines):
        """Parses ground truth from one of the .csv files

        Args:
            file_lines {list}: A list of lines from the ground truth file

        Returns:
            {list}: A list of FrameData objects containing the information for each line from the ground truth file
        """
        frame_data = []
        for current_line in file_lines:
            # form a new frame data struct using the values from this line
            frame_data.append(FrameData.from_ground_truth_line(current_line))
        return frame_data

    def parse_pandata_lines(self, file_lines):
        """Parses ground truth from one of the PANDATA .csv files. These files
        augment the information provided in the larger .csv files.

        Args:
            file_lines {list}: A list of lines from the PANDATA ground truth file

        Returns:
            [list]: A list of PanFrameData objects containing the information for each line from the ground truth file
        """
        pan_frame_data = []
        for current_line in file_lines:
            # form a new frame data struct using the values from this line
            pan_frame_data.append(PanFrameData.from_pandata_ground_truth_line(current_line))
        return pan_frame_data
