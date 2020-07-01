"""Stores frame data as parsed from the csv ground truth files"""


class FrameData:
    """This class stores the ground truth data as found in the '<DATE TIME>.csv' type files
    """

    def __init__(self):
        # the frame number
        self.frame_num = -1
        # The stage the program was in when this frame was captured
        self.stage = -1
        # the time that has elapsed since the program started
        self.elapsed_time = -1
        # The X and Y position on the XY Table
        self.x_pos = -1
        self.y_pos = -1
        # the yaw, pitch and roll from the IC2
        self.ic2_yaw = -1
        self.ic2_pitch = -1
        self.ic2_roll = -1

        # The following entries are only available on recordings made after 10am on the 24th
        # as that is when i started recording those values
        # The Pan position and Tilt position as measured from the Pan/Tilt Unit
        self.pan = -1
        self.tilt = -1
        # the time at which the pan and tilt measurements were taken
        self.pan_time = -1
        self.tilt_time = -1
        # Uses the information from the time stamps to accurately determine the frames pose
        #FrameData * CalculatePose(FrameData PreviousFrame)

    @classmethod
    def from_ground_truth_line(cls, line):
        """Takes in a single line of ground truth, parses it and returns a FrameData
        instance containing these values"""
        # tokenize each line based on comma character
        tokens = line.rstrip('\n').split(',')
        if len(tokens) < 8:
            print(f'ERROR: Line from ground truth only has {len(line)} values, should have 8')
            return
        instance = cls()
        instance.frame_num = int(tokens[0])
        instance.x_pos = int(tokens[1])
        instance.y_pos = int(tokens[2])
        instance.ic2_yaw = float(tokens[3])
        instance.ic2_pitch = float(tokens[4])
        instance.ic2_roll = float(tokens[5])
        instance.elapsed_time = float(tokens[6])
        instance.stage = int(tokens[7])

        if len(tokens) != 12:  # this might be one of the files where the data from the pan/tilt unit was not recorded
            instance.pan = 0
            instance.pan_time = -99
            instance.tilt = 0
            instance.tilt_time = -99
        else:
            instance.pan = int(tokens[8])
            instance.pan_time = float(tokens[9])
            instance.tilt = int(tokens[10])
            instance.tilt_time = float(tokens[11])

        return instance

class PanFrameData:
    """This class stores the pan data as found in the '<DATE TIME>_PANDATA.csv' type files. This stores the
    PTU data from various stages during the given capture run
    """

    def __init__(self):
        # The frame number
	    self.frame_num = -1
	    # The line number in the source where this pan or tilt started
	    self.line_num = -1
	    # The stage the program was in when this frame was captured
	    self.stage = -1
	    # the time that has elapsed since the program started
	    self.elapsed_time = -1
	    # a 0 indicates this frame records a Pan movement a 1 indicates a Tilt
	    self.is_tilt = False

    @classmethod
    def from_pandata_ground_truth_line(cls, line):
        """Takes in a single line of ground truth PANDATA, parses it and returns a PanFrameData
        instance containing these values"""
        # tokenize each line based on comma character
        tokens = line.rstrip('\n').split(',')
        if len(tokens) < 5:
            print(f'ERROR: Line from PANDATA file only has {len(line)} values, should have 5')
            return
        instance = cls()
        instance.frame_num = int(tokens[0])
        instance.line_num = int(tokens[1])
        instance.elapsed_time = float(tokens[2])
        instance.is_tilt = int(tokens[3]) == 1
        instance.stage = int(tokens[4])

        return instance
