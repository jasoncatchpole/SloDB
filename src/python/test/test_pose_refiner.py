"""Unit tests covering the refinement of pose data by fusing the frame data with the pandata.

Can run this using: python -m unittest src\test\test_pose_refiner.py"""

import unittest
import sys
import math
sys.path.append('.\src')
from frame_data import FrameData, PanFrameData
from ground_truth_parser import GroundTruthParser
from pose_refiner import PoseRefiner

class TestPoseRefiner(unittest.TestCase):
    def test_all_seven_stages_26th(self):
        file_parser = GroundTruthParser()
        stage8_file_lines = file_parser.read_file("src/python/test/26-1-2006_9-37-stage7.csv")
        stage8_frame_data = file_parser.parse_lines(stage8_file_lines)
        original_file_lines = file_parser.read_file("src/python/test/26-1-2006_9-37.csv")
        original_pandata_lines = file_parser.read_file("src/python/test/26-1-2006_9-37_PANDATA.csv")
        original_frame_data = file_parser.parse_lines(original_file_lines)
        original_pan_data = file_parser.parse_pandata_lines(original_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(original_frame_data, original_pan_data)

        for frame_index, current_frame in enumerate(stage8_frame_data):
            print(f'Frame {current_frame.frame_num}: Reference = {current_frame}, live = {original_frame_data[frame_index]}')
            self.assertTrue(current_frame == original_frame_data[frame_index])

    def test_all_seven_stages_8th(self):
        file_parser = GroundTruthParser()
        stage8_file_lines = file_parser.read_file("src/python/test/8-2-2006_16-29-stage7.csv")
        stage8_frame_data = file_parser.parse_lines(stage8_file_lines)
        original_file_lines = file_parser.read_file("src/python/test/8-2-2006_16-29.csv")
        original_pandata_lines = file_parser.read_file("src/python/test/8-2-2006_16-29_PANDATA.csv")
        original_frame_data = file_parser.parse_lines(original_file_lines)
        original_pan_data = file_parser.parse_pandata_lines(original_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(original_frame_data, original_pan_data)

        for frame_index, current_frame in enumerate(stage8_frame_data):
            print(f'Frame {current_frame.frame_num}: Reference = {current_frame}, live = {original_frame_data[frame_index]}')
            self.assertTrue(current_frame == original_frame_data[frame_index])

    def test_all_seven_stages_cutdown_8th(self):
        file_parser = GroundTruthParser()
        stage8_file_lines = file_parser.read_file("src/python/test/23-1-2006_14-9-stage7.csv")
        stage8_frame_data = file_parser.parse_lines(stage8_file_lines)
        original_file_lines = file_parser.read_file("src/python/test/23-1-2006_14-9.csv")
        original_pandata_lines = file_parser.read_file("src/python/test/23-1-2006_14-9_PANDATA.csv")
        original_frame_data = file_parser.parse_lines(original_file_lines)
        original_pan_data = file_parser.parse_pandata_lines(original_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(original_frame_data, original_pan_data)

        for frame_index, current_frame in enumerate(stage8_frame_data):
        #for frame_index in range(1060, len(stage8_frame_data)):
            current_frame = stage8_frame_data[frame_index]
            print(f'Frame {current_frame.frame_num}: Reference = {current_frame}, live = {original_frame_data[frame_index]}')
            self.assertTrue(current_frame == original_frame_data[frame_index])

if __name__ == '__main__':
    unittest.main()
