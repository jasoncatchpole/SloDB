"""The PoseCalculator combines the pandata and the frame data into improved ground truth
measurements"""

class PoseCalculator:
    def __init__(self):
        

    def fuse_ground_truth(self, frames, pan_frames):
        """Takes in the info read from the two types of ground truth files and consolidates the
        info into a new and improves frames structure that exploits the pandata"""
        if len(frames) <= 0 or len(pan_frames) <= 0:
            print('ERROR: Cannot fuse ground truth as frames structure provided was empty')
            return None
        if len(pan_frames) != 31:
            print(f'ERROR: Cannot fuse ground truth as pandata only"\
                " contains {len(pan_data)} entries (should be 31)')
            return None

        if frames[0].pan_time == -99 and frames[0].tilt_time == -99:
            # this means we have an instance where we don't have the pan & tilt times recorded
            # (this was on the first day where these were not added yet)
            return self.fuse_ground_truth_fairly_accurate(frames, pan_frames)

        ##################################################################################
	    # Stage 1 (2 pan frames) - this stage involves moving to the left while panning right,
	    # then moving to the right while panning left
        # Please note we skip the first two entries in pan_frames these entries are during initialization
        pan_frame_index = 2
	    self.general_interpolation(frames, pan_frames[pan_frame_index].frame_fum, 550,\
            pan_frames[pan_frame_index].elapsed_time, -670, pan_frames[pan_frame_index+1].FrameNum\
                ,pan_frames[pan_frame_index].is_tilt, -200)

        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_fum, -670,\
            pan_frames[pan_frame_index].elapsed_time, 550, pan_frames[pan_frame_index+1].FrameNum\
                ,pan_frames[pan_frame_index].is_tilt, -200)
        
        
    def general_interpolation(frames, start_frame, start_value, start_time, end_position,\
        last_frame, is_tilt, other_val)
        """Performs interpolation between BLAH
        """
        
    def fuse_ground_truth_fairly_accurate(self, frames, pan_data):
        """Used for data captured on the first day before I added the extra pan & tilt info.
        This function does its best with the slightly reduced information"""
