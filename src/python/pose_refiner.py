"""The PoseRefiner combines the pandata and the frame data into improved ground truth
measurements"""

import math

class PoseRefiner:
    def __init__(self):
        pass

    def refine_frame_poses(self, frames, pan_frames):
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
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 550,\
            pan_frames[pan_frame_index].elapsed_time, -670, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)

        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -670,\
            pan_frames[pan_frame_index].elapsed_time, 550, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)

        ##################################################################################
        # Stage 2 (8 pan frames) - This stage involves doing a jagged (Fast) left right pan
        # with no movement position wise
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 550,\
            pan_frames[pan_frame_index].elapsed_time, 0, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 0,\
            pan_frames[pan_frame_index].elapsed_time, -580, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -580,\
            pan_frames[pan_frame_index].elapsed_time, 380, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 380,\
            pan_frames[pan_frame_index].elapsed_time, -580, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -580,\
            pan_frames[pan_frame_index].elapsed_time, 380, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 380,\
            pan_frames[pan_frame_index].elapsed_time, -580, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -580,\
            pan_frames[pan_frame_index].elapsed_time, 380, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 380,\
            pan_frames[pan_frame_index].elapsed_time, 0, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200)

        ##################################################################################
        # Stage 3 (6 pan frames) - This stage involves doing a up down tilt with no movement
        # position wise
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -200,\
            pan_frames[pan_frame_index].elapsed_time, -580, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -580,\
            pan_frames[pan_frame_index].elapsed_time, 150, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 150,\
            pan_frames[pan_frame_index].elapsed_time, -580, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0)
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -580,\
            pan_frames[pan_frame_index].elapsed_time, 150, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0)

        # at this stage the up down tilt is really complete, now we are moving into position to do the Circular Motion,
        # i.e the one with the diamond shape
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 150,\
            pan_frames[pan_frame_index].elapsed_time, -200, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 0,\
            pan_frames[pan_frame_index].elapsed_time, 400, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, -200) # this is a pan

        ##################################################################################
        # Stage 4 (6 pan frames) - This stage performs a diagonal motion which involves both 
        # a pan and tilt at the same time. I need a special method to deal with this as there
        # isn't any pan and tilt measurements for this (couldn't capture them for the command
        # type EXECUTE_UPON_IMMEDIATE_OR_AWAIT which was required for doing simultaneous pan 
        # and tilt movements.
        pan_frame_index = pan_frame_index + 1
        self.diagonal_interpolation(frames, pan_frames[pan_frame_index].frame_num,\
            pan_frames[pan_frame_index+1].frame_num, 400, -200,\
            pan_frames[pan_frame_index].elapsed_time, pan_frames[pan_frame_index+1].elapsed_time,\
            -100, 300)
        pan_frame_index = pan_frame_index + 1
        self.diagonal_interpolation(frames, pan_frames[pan_frame_index].frame_num,\
            pan_frames[pan_frame_index+1].frame_num, -100, 300,\
            pan_frames[pan_frame_index].elapsed_time, pan_frames[pan_frame_index+1].elapsed_time,\
            -600, -200)
        pan_frame_index = pan_frame_index + 1
        self.diagonal_interpolation(frames, pan_frames[pan_frame_index].frame_num,\
            pan_frames[pan_frame_index+1].frame_num, -600, -200,\
            pan_frames[pan_frame_index].elapsed_time, pan_frames[pan_frame_index+1].elapsed_time,\
            -100, -700)
        pan_frame_index = pan_frame_index + 1
        self.diagonal_interpolation(frames, pan_frames[pan_frame_index].frame_num,\
            pan_frames[pan_frame_index+1].frame_num, -100, -700,\
            pan_frames[pan_frame_index].elapsed_time, pan_frames[pan_frame_index+1].elapsed_time,\
            400, 0)
        
        # at this stage we have technically finished the diagonal movements now we are just
        # setting up for the next stage
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 0,\
            pan_frames[pan_frame_index].elapsed_time, 150, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 400) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        if frames[pan_frames[pan_frame_index].frame_num].pan == 0:
            # this shouldn't be zero it should be 400 (or something else other than 0)
            frames[pan_frames[pan_frame_index].frame_num].pan = 400
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 400,\
            pan_frames[pan_frame_index].elapsed_time, 0, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 150) # this is a pan

        ##################################################################################
        # Stage 5 (2 pan frames) - Moving Up/Down Tilt
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 150,\
            pan_frames[pan_frame_index].elapsed_time, -550, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -550,\
            pan_frames[pan_frame_index].elapsed_time, 200, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt

        ##################################################################################
        # Stage 6 (4 pan frames) - The Up/Down while moving forward
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 200,\
            pan_frames[pan_frame_index].elapsed_time, -400, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -400,\
            pan_frames[pan_frame_index].elapsed_time, 200, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, 200,\
            pan_frames[pan_frame_index].elapsed_time, -400, pan_frames[pan_frame_index+1].frame_num,\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        pan_frame_index = pan_frame_index + 1
        # this next movement can be skipped since it goes from -400 tilt to -400 tilt, for some reason it was the exact same movement

        ##################################################################################
        # Stage 7 (1 pan frame) - 
        pan_frame_index = pan_frame_index + 1
        self.general_interpolation(frames, pan_frames[pan_frame_index].frame_num, -400,\
            pan_frames[pan_frame_index].elapsed_time, 70, len(frames),\
            pan_frames[pan_frame_index].is_tilt, 0) # this is a tilt
        
    def general_interpolation(self, frames, begin_frame, begin_value, begin_time, end_position,\
        last_frame, is_tilt, other_val) -> int:
        """Performs interpolation between BLAH
        """
        if begin_frame > len(frames):
            return 0

        start_time = begin_time
        end_time = 0.0
        start_val = begin_value
        end_val = 0
        end_frame = 0

        # now we need to find the first measurement so we can fill in the endTime and endVal
        # values so we can interpolate between these values. Iterate over all the frames starting
        # from the start_frame number
        frame_index = begin_frame
        while frame_index < len(frames):
            while frame_index < len(frames):
                if is_tilt:
                    if frames[frame_index].tilt != start_val or frames[frame_index].frame_num == last_frame:
                        end_frame = frames[frame_index].frame_num
                        end_time = frames[frame_index].tilt_time
                        end_val = frames[frame_index].tilt
                        break
                else:
                    if frames[frame_index].pan != start_val or frames[frame_index].frame_num == last_frame:
                        if frames[frame_index].pan_time == 0: # this means we are either approaching the circular movement or have just finised it but the values have not caught up yet
                            end_frame = last_frame
                            end_time = frames[frame_index].elapsed_time
                            end_val = end_position
                            if frames[frame_index].frame_num != last_frame: # this means we reached the circular motion point but there are frames in between which are 0
                                for tilt_index in range(frame_index, len(frames)):
                                    if frames[tilt_index].frame_num == last_frame:
                                        break # break out of inner while loop
                                    frames[tilt_index].tilt = other_val
                        else:
                            end_frame = frames[frame_index].frame_num
                            end_time = frames[frame_index].pan_time # normally the panTime and the Elapsed time are the same but sometimes panTime is more accurate
                            end_val = frames[frame_index].pan
                        break # break out of inner while loop
                frame_index+=1

            # now that we have found the location of the frames to interpolate between do the interpolation
            # CALL A FUNCTION HERE WHICH INTERPOLATES BETWEEN 2 GIVEN FRAMES/TIMES AND POSITIONS
            self.interframe_interpolation(frames, begin_frame, end_frame, start_time, end_time, start_val, end_val, is_tilt, other_val)

            # now set the start as the end of the last interpolation and do the next segment
            start_time = end_time
            start_val = end_val
            begin_frame = end_frame

            # check to see if we have reached the end of this run or not
            if start_val == end_position or begin_frame == last_frame:
                # we will stop now but first check to make sure the very last measurement doesn't need correcting
                #if( (*iter).elapsedTime < (*iter).panTime)

                # break out of the outer while loop
                break

        return 1

    def interframe_interpolation(self, frames, start_frame, end_frame, start_time, end_time,\
        begin_val, end_val, is_tilt, other_val) -> None:
        """given the start and end frames (and hence the values), calculate all the values for the
        frames in between"""
        # now calculate the time difference and the difference in the pose
        time_diff = end_time - start_time

        start_val = begin_val
        pos_diff = end_val - start_val

        frame_index = start_frame
        while frame_index < end_frame:
            # calculate the time difference between the frame we are trying to calculate and the
            # first frame
            cur_time_diff = frames[frame_index].elapsed_time - start_time

            # now find the % of the whole time between the actual measurements so we can find the
            # value for this frame
            per_diff = cur_time_diff / time_diff

            if is_tilt:
                frames[frame_index].tilt = (per_diff * pos_diff) + start_val
                frames[frame_index].tilt_time = frames[frame_index].elapsed_time
                if frames[frame_index].pan != other_val:
                    frames[frame_index].pan = other_val
            else:
                frames[frame_index].pan = (per_diff * pos_diff) + start_val
                frames[frame_index].pan_time = frames[frame_index].elapsed_time
                if frames[frame_index].tilt != other_val:
                    frames[frame_index].tilt = other_val

            #result = (per_diff * pos_diff) + start_val
            #curFrame = frames[frame_index].frame_num

            frame_index += 1

    def diagonal_interpolation(self, frames, begin_frame, end_frame, begin_val_pan, begin_val_tilt,\
        begin_time, end_time, end_pos_pan, end_pos_tilt):
        """Does the interpolation between frames for the diagonal movements where there is no
        time stamps for pan and tilt data"""
        pan_speed = 100
        tilt_speed = 100

        # the pan and tilt speed were exactly the same for this sequence so as long as the
        # distance is the same it will finish at the same time
        if math.fabs(end_pos_pan - begin_val_pan) == math.fabs(end_pos_tilt - begin_val_tilt):
            pan_speed = (end_pos_pan - begin_val_pan) / (end_time - begin_time)
            tilt_speed = pan_speed
        else:
            # they aren't the same so the one which has to travel the furtherest will take the
            # longest
            if math.fabs(end_pos_pan - begin_val_pan) > math.fabs(end_pos_tilt - begin_val_tilt):
                pan_speed = (end_pos_pan - begin_val_pan) / (end_time - begin_time)
            else:
                tilt_speed = (end_pos_tilt - begin_val_tilt) / (end_time - begin_time)

        if end_pos_pan < begin_val_pan and pan_speed > 0:
            # make sure the speed is negative
            pan_speed = -pan_speed
        elif end_pos_pan >= begin_val_pan and pan_speed < 0:
            pan_speed = -pan_speed
        if end_pos_tilt < begin_val_tilt and tilt_speed > 0:
            # make sure the speed is negative
            tilt_speed = -tilt_speed
        elif end_pos_tilt >= begin_val_tilt and tilt_speed < 0:
            tilt_speed = -tilt_speed

        # TODO: UP TO LINE 567 of PoseRefiner.cpp
        frame_index = begin_frame
        while frame_index < end_frame:
            #cur_frame_num = frames[frame_index].frame_num
            frames[frame_index].pan = pan_speed * (frames[frame_index].elapsed_time - begin_time) + begin_val_pan

            if not self.is_between(frames[frame_index].pan, begin_val_pan, end_pos_pan):
                if frames[frame_index].elapsed_time < begin_time: # big problem, well kinda big, actually not big at all
                    print("Error: The beginning of this diagonal movement is before the first elapsed time")
                    frames[frame_index].pan = begin_val_pan
                else:
                    frames[frame_index].pan = end_pos_pan

            frames[frame_index].tilt = tilt_speed * (frames[frame_index].elapsed_time - begin_time) + begin_val_tilt
            if not self.is_between(frames[frame_index].tilt, begin_val_tilt, end_pos_tilt):
                if frames[frame_index].elapsed_time < begin_time: # big problem, well kinda big, actually not big at all
                    print("Error: The beginning of this diagonal movement is before the first elapsed time")
                    frames[frame_index].tilt = begin_val_tilt
                else:
                    frames[frame_index].tilt = end_pos_tilt

            frame_index += 1

    def is_between(self, cur_val, start_val, end_val) -> bool:
        if end_val < start_val:  # this means we have a negative velocity
            if cur_val > start_val or cur_val < end_val:
                return False
        else: # the start is less than the end
            if cur_val < start_val or cur_val > end_val:
                return False
        return True
        
    def fuse_ground_truth_fairly_accurate(self, frames, pan_data):
        """Used for data captured on the first day before I added the extra pan & tilt info.
        This function does its best with the slightly reduced information"""
        pass
