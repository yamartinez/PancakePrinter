# Sources: https://learnopencv.com/edge-detection-using-opencv/
#          https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/

import sys
import cv2

# Read the original image
difficulty = sys.argv[1]
img_path = 'Edge-Detection-Test-' + difficulty + '.jpg'
img = cv2.imread(img_path)
# Resize the image to decrease the resolution
img_resize = img
scale_percent = 80 # percent of original size
width = int(img.shape[1] * scale_percent / 100)
height = int(img.shape[0] * scale_percent / 100)
dim = (width, height)
while (width > 300 or height > 300):
    img_resize = cv2.resize(img_resize, dim, interpolation = cv2.INTER_AREA)
    width = int(img_resize.shape[1] * scale_percent / 100)
    height = int(img_resize.shape[0] * scale_percent / 100)
    dim = (width, height)
# Display the resized image
cv2.imshow('Original', img_resize)
cv2.waitKey(0)

# Contrast colors in image for better edge detection?

# Convert to graycsale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Blur the image for better edge detection
img_blur = cv2.GaussianBlur(img_gray, (3,3), 0)

# Canny Edge Detection
edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200) # Canny Edge Detection
# Display Canny Edge Detection Image
cv2.imshow('Canny Edge Detection', edges) # edges is a binary mask of the detected edges
cv2.waitKey(0)
 
cv2.destroyAllWindows()