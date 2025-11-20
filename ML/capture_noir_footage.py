import cv2
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder

# Start up the NoIR Pi Camera
picam2 = Picamera2()
video_config = picam2.create_video_configuration()
picam2.configure(video_config)

# Set up encoder for OpenCV recording
# This version uses video; for the actual project, we'll need photos
# But OpenCV can do that pretty easy, too
encoder = H264Encoder()
output_file = "noir_video.h264"

picam2.start()
picam2.start_recording(encoder, output_file)

# OpenCV creates window on desktop verison of Raspberry Pi
# Just done for PDR; looks cool
cv2.namedWindow("NoIR Camera Live", cv2.WINDOW_AUTOSIZE)

try:
	while True:
		# Captures a frame
		frame = picam2.capture_array()
		
        # Shows the frame on the video
		cv2.imshow("NoIR Camera Live", frame)

        # Closes the OpenCV streaming video if q is pressed
		if cv2.waitKey(1) & 0xFF == ord('q'):
			break

finally:
	# Closes the program
	picam2.stop_recording()
	picam2.stop()
	cv2.destroyAllWindows()

# Tells user where video was saved
print(f"Video saved to {output_file}")

