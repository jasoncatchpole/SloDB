"""Unit tests covering the parsing of ground truth data.

Can run this using: python -m unittest src\test\test_ground_truth_parse.py"""

import unittest
import sys
sys.path.append('.\src')
from frame_data import FrameData, PanFrameData

class TestFrameData(unittest.TestCase):

    def test_frame_data_line_parse(self):
        """Tests the 'from_ground_truth_line()' function in FrameData to
        ensure that FrameData instances can be constructed out of a line
        of comma separated values the same as what would be read from a file"""
        line = "4,0,-3021,228.153,81.0342,169.79,30.218,0,550,19.062,-200,18.406"
        test = FrameData.from_ground_truth_line(line)
        self.assertEqual(test.frame_num, 4)
        self.assertEqual(test.x_pos, 0)
        self.assertEqual(test.y_pos, -3021)
        self.assertEqual(test.ic2_yaw, 228.153)
        self.assertEqual(test.ic2_pitch, 81.0342)
        self.assertEqual(test.ic2_roll, 169.79)
        self.assertEqual(test.elapsed_time, 30.218)
        self.assertEqual(test.stage, 0)
        # the following are in the ground truth line so these values will just be defaults
        self.assertEqual(test.pan, 550)
        self.assertEqual(test.pan_time, 19.062)
        self.assertEqual(test.tilt, -200)
        self.assertEqual(test.tilt_time, 18.406)

    def test_frame_data_line_parser_cut_down(self):
        """Same as 'test_frame_data_line_parse' except this time we only pass
        the shorted form of the ground truth data. We just had these on the first
        day of capture"""
        line = "3,0,-2751,279.143,74.4846,102.874,26.844,0"
        test = FrameData.from_ground_truth_line(line)
        self.assertEqual(test.frame_num, 3)
        self.assertEqual(test.x_pos, 0)
        self.assertEqual(test.y_pos, -2751)
        self.assertEqual(test.ic2_yaw, 279.143)
        self.assertEqual(test.ic2_pitch, 74.4846)
        self.assertEqual(test.ic2_roll, 102.874)
        self.assertEqual(test.elapsed_time, 26.844)
        self.assertEqual(test.stage, 0)
        # the following are in the ground truth line so these values will just be defaults
        self.assertEqual(test.pan, 0)
        self.assertEqual(test.pan_time, -99)
        self.assertEqual(test.tilt, 0)
        self.assertEqual(test.tilt_time, -99)

    def test_pandata_parser(self):
        """Tests the functionality for parsing out a line from a PANDATA file"""
        line = "2004,157,110.218,1,3"
        test = PanFrameData.from_pandata_ground_truth_line(line)
        self.assertEqual(test.frame_num, 2004)
        self.assertEqual(test.line_num, 157)
        self.assertEqual(test.elapsed_time, 110.218)
        self.assertEqual(test.is_tilt, True)
        self.assertEqual(test.stage, 3)

if __name__ == '__main__':
    unittest.main()