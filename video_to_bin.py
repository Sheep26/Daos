import cv2
import numpy as np

video_file = "input.mp4"
output_file = "video.bin"
resize_width, resize_height = 400, 400

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