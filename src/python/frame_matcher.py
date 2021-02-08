"""The PoseCalculator combines the pandata and the frame data into improved ground truth
measurements"""

import math

class FrameMatcher:
    def __init__(self, all_frames):
        """Precomputes all the information required to match any frame from the reference
        video to any of the other videos.

        Args:
            all_frames (a array of an array of frame_data objects): Contains all the pose refined
                frame_data objects for each of the videos we are matching (the first array contains
                the frame_data from the reference video)
        """
        self._all_frames = all_frames

        self._stage_list = []
        self.build_stage_list()
        

    def build_stage_list(self):
        """Stores the locations of the starts and ends of each stage for each video file so that
        they can be searched more quickly"""
        
        # go through each of the 8 stages
        for current_stage in range(0, 8):
            stage_pairs = []

            for current_video_frames in self._all_frames:
                start = 0
                end = 0

                start, end = self.find_stage_start_end(current_stage, current_video_frames, 0)

                matching_pair = (start, end)

                stage_pairs.append(matching_pair)

            self._stage_list.append(stage_pairs)

    def seek(self, main_pos) -> tuple:
        """Determines the seek positions in each video in order to match (as closesly as possible) the frame in
        the reference video (with index 'main_pos'). Returns a list of seek positions for the videos being matched
        to the reference as well as their corresponding distances indicating how close each frame is to the reference
        frame"""
        seek_positions = []
        distances = []

        # make sure the number of entires (videos) in VideoFrames is >= 2
        if len(self._all_frames) < 2:
            print("Trying to seek and sync when there aren't even enough videos to sync")
            return seek_positions
        if main_pos > len(self._all_frames[0]):
            print("Trying to seek to a location in the main video which is out of range of the "\
                "video (the video doesn't have this many frames)")
            return seek_positions

        # a seek has occurred, where mainPos indicates the position we are seeking to in the main
        # video which is in position 0 of VideoFrames

        # go through each combination of main frame with each of the remaining videos finding the
        # closest frame to the main one
        main_frame = self._all_frames[0][main_pos]

        for other_video_index in range(1, len(self._all_frames)):
            index, closest_distance = self.locate_closest_frame(main_frame, other_video_index)

            seek_positions.append(index)
            distances.append(closest_distance)

        return seek_positions, distances

    # Searches through 'frames' to find the closest frame in pose to 'mainFrame'
    def locate_closest_frame(self, main_frame, frame_set_index) -> int:
        match_index = -1
        closest_distance = 999999999

        # compute the percentage through the stage the reference is
        if main_frame.frame_num - self._stage_list[main_frame.stage][0][0] == 0:
            percentage_through = 0
        else:    
            percentage_through = (main_frame.frame_num - self._stage_list[main_frame.stage][0][0]) / (self._stage_list[main_frame.stage][0][1] - self._stage_list[main_frame.stage][0][0])

        # first get the rough position by using the stage of the main frame and getting the
        # corresponding stage in the other videos
        begin_frame_stage = self._stage_list[main_frame.stage][frame_set_index][0]
        end_frame_stage = self._stage_list[main_frame.stage][frame_set_index][1]
        frame_range = end_frame_stage - begin_frame_stage

        # now take the reference's position through the range and apply this to the other video
        if main_frame.stage == 0:
            percentage_through_start = 0
            percentage_through_end = 1.0
        else:
            percentage_through_start = max(0, percentage_through - 0.1)
            percentage_through_end = min(1.0, percentage_through + 0.1)

        begin_frame = math.floor(begin_frame_stage + (percentage_through_start * frame_range))
        end_frame = math.floor(begin_frame_stage + (percentage_through_end * frame_range))
        
        for current_index in range(begin_frame, end_frame):
            current_distance = FrameMatcher.distance(main_frame, self._all_frames[frame_set_index][current_index])

            if current_distance < closest_distance:
                closest_distance = current_distance
                match_index = self._all_frames[frame_set_index][current_index].frame_num

        if closest_distance > 40:
            match_index = -1

        # returns the index of the closest frame in 'frames' to 'mainFrame', returns -1 if no
        # match found
        return match_index, closest_distance

    # Calculates the distance between 2 frames
    @staticmethod
    def distance(frame1, frame2) -> float:
        dist = 0.0

        # calculate the euclidean distance between the two positions and orientations
        dist_ori = math.pow(frame1.pan - frame2.pan, 2) + math.pow(frame1.tilt - frame2.tilt, 2)
        dist_ori = math.sqrt(dist_ori)

        dist_pos = math.pow(frame1.x_pos - frame2.x_pos, 2) + math.pow(frame1.y_pos - frame2.y_pos, 2)
        dist_pos = math.sqrt(dist_pos)

        # position has less of an impact than orientation, also position has a larger range
        # therefore scale the effect of position difference
        dist = dist_ori + (0.1 * dist_pos)

        return dist

    def find_stage_start_end(self, stage, frames, offset):
        """Locates the start and end frame for a given stage in a set of frames"""
        start = end = 0

        frame_index = offset

        # first find the start of the stage
        while frame_index < len(frames):
            if frames[frame_index].stage == stage:
                start = frames[frame_index].frame_num
                break
            frame_index += 1

        # now find the end
        while frame_index < len(frames):
            if frames[frame_index].stage != stage:
                end = frames[frame_index].frame_num - 1
                break
            frame_index += 1

        # this means we were searching for stage number 8 which would hit the end before it could
        # set the end frame value
        if end == 0:
            end = len(frames) - 1

        return start, end
