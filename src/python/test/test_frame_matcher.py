"""Unit tests covering the frame matcher.

Can run this using: python -m unittest src\test\test_frame_matcher.py"""

import unittest
import sys
import math
sys.path.append('.\src')
from frame_data import FrameData, PanFrameData
from ground_truth_parser import GroundTruthParser
from frame_matcher import FrameMatcher
from pose_refiner import PoseRefiner

class TestFrameMatcher(unittest.TestCase):
    def test_frame_matcher(self):
        file_parser = GroundTruthParser()

        first_file_lines = file_parser.read_file("src/python/test/26-1-2006_9-37.csv")
        first_pandata_lines = file_parser.read_file("src/python/test/26-1-2006_9-37_PANDATA.csv")
        first_frame_data = file_parser.parse_lines(first_file_lines)
        first_pan_data = file_parser.parse_pandata_lines(first_pandata_lines)

        second_file_lines = file_parser.read_file("src/python/test/8-2-2006_16-29.csv")
        second_pandata_lines = file_parser.read_file("src/python/test/8-2-2006_16-29_PANDATA.csv")
        second_frame_data = file_parser.parse_lines(second_file_lines)
        second_pan_data = file_parser.parse_pandata_lines(second_pandata_lines)

        third_file_lines = file_parser.read_file("src/python/test/23-1-2006_14-9.csv")
        third_pandata_lines = file_parser.read_file("src/python/test/23-1-2006_14-9_PANDATA.csv")
        third_frame_data = file_parser.parse_lines(third_file_lines)
        third_pan_data = file_parser.parse_pandata_lines(third_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(first_frame_data, first_pan_data)
        refiner.refine_frame_poses(second_frame_data, second_pan_data)
        refiner.refine_frame_poses(third_frame_data, third_pan_data)

        all_frames = []
        all_frames.append(first_frame_data)
        all_frames.append(second_frame_data)
        all_frames.append(third_frame_data)

        frame_matcher = FrameMatcher(all_frames)
        seek_positions, distances = frame_matcher.seek(5)
        self.assertEqual(len(seek_positions), 2)
        self.assertEqual(seek_positions[0], 90)
        self.assertEqual(seek_positions[1], 0)

        seek_positions, distances = frame_matcher.seek(25)
        self.assertEqual(seek_positions[0], 111)
        self.assertEqual(seek_positions[1], 16)

        seek_positions, distances = frame_matcher.seek(100)
        self.assertEqual(seek_positions[0], 187)
        self.assertEqual(seek_positions[1], 81)

        seek_positions, distances = frame_matcher.seek(200)
        self.assertEqual(seek_positions[0], 288)
        self.assertEqual(seek_positions[1], 178)

        seek_positions, distances = frame_matcher.seek(8409) # in stage 8
        self.assertEqual(seek_positions[0], 8419)
        self.assertEqual(seek_positions[1], 7751)

    def test_frame_matcher_failure_case(self):
        """This test covers a failure case detected when using the GUI"""
        file_parser = GroundTruthParser()

        first_file_lines = file_parser.read_file("src/python/test/26-1-2006_12-40.csv")
        first_pandata_lines = file_parser.read_file("src/python/test/26-1-2006_12-40_PANDATA.csv")
        first_frame_data = file_parser.parse_lines(first_file_lines)
        first_pan_data = file_parser.parse_pandata_lines(first_pandata_lines)

        second_file_lines = file_parser.read_file("src/python/test/25-1-2006_10-49.csv")
        second_pandata_lines = file_parser.read_file("src/python/test/25-1-2006_10-49_PANDATA.csv")
        second_frame_data = file_parser.parse_lines(second_file_lines)
        second_pan_data = file_parser.parse_pandata_lines(second_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(first_frame_data, first_pan_data)
        refiner.refine_frame_poses(second_frame_data, second_pan_data)

        all_frames = []
        all_frames.append(first_frame_data)
        all_frames.append(second_frame_data)

        frame_matcher = FrameMatcher(all_frames)
        seek_positions, distances = frame_matcher.seek(429)
        print(f'MATCHES ARE {seek_positions}')
        self.assertEqual(len(seek_positions), 1)
        self.assertEqual(seek_positions[0], 295)

    def test_frame_matcher_failure_case2(self):
        """Another failure case detected via GUI"""
        file_parser = GroundTruthParser()

        first_file_lines = file_parser.read_file("src/python/test/27-1-2006_11-48.csv")
        first_pandata_lines = file_parser.read_file("src/python/test/27-1-2006_11-48_PANDATA.csv")
        first_frame_data = file_parser.parse_lines(first_file_lines)
        first_pan_data = file_parser.parse_pandata_lines(first_pandata_lines)

        second_file_lines = file_parser.read_file("src/python/test/27-1-2006_17-0.csv")
        second_pandata_lines = file_parser.read_file("src/python/test/27-1-2006_17-0_PANDATA.csv")
        second_frame_data = file_parser.parse_lines(second_file_lines)
        second_pan_data = file_parser.parse_pandata_lines(second_pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(first_frame_data, first_pan_data)
        refiner.refine_frame_poses(second_frame_data, second_pan_data)

        all_frames = []
        all_frames.append(first_frame_data)
        all_frames.append(second_frame_data)

        frame_matcher = FrameMatcher(all_frames)
        seek_positions, distances = frame_matcher.seek(207)
        print(f'MATCHES ARE {seek_positions}')
        self.assertEqual(len(seek_positions), 1)
        self.assertEqual(seek_positions[0], 67)

        seek_positions, distances = frame_matcher.seek(208)
        print(f'MATCHES ARE {seek_positions}')
        self.assertEqual(len(seek_positions), 1)
        self.assertEqual(seek_positions[0], 69)



if __name__ == '__main__':
    unittest.main()