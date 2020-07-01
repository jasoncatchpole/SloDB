"""Entry point of application"""

from ground_truth_parser import GroundTruthParser

""" Links for some python GUI related functionality:

https://stackoverflow.com/questions/32342935/using-opencv-with-tkinter
https://www.pyimagesearch.com/2016/05/30/displaying-a-video-feed-with-opencv-and-tkinter/
https://www.youtube.com/watch?v=-Dp2_X9q7GU
"""

def main():
    print("Hello World!")

    file_parser = GroundTruthParser()
    file_lines = file_parser.read_file("C:\\Code\\SloDB\\src\\test\\23-1-2006_14-9.csv")
    print(f'First file line is {file_lines[0]}')

    # now lets convert these file lines into frame data structures
    frame_data = file_parser.parse_lines(file_lines)
    print(f'First frame data is {frame_data[0].x_pos}')

if __name__ == "__main__":
    main()