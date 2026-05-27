import cv2
import numpy as np
import sys

video_file = sys.argv[1]
output_file = sys.argv[2]
resize_width, resize_height = 640, 480

cap = cv2.VideoCapture(video_file)
frames = []

with open(output_file, "wb") as f:
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frame = cv2.resize(frame, (resize_width, resize_height))
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        bw = (gray >= 128).astype(np.uint8)
        packed = np.packbits(bw, axis=1)
        f.write(packed.tobytes())

cap.release()