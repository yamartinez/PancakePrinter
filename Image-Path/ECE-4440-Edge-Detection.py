# Sources: https://learnopencv.com/edge-detection-using-opencv/
#          https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/

import sys
import cv2

# Read the original image
name = sys.argv[1]
img_path = 'Edge-Detection-Test-' + name + '.jpg'
try:
    img = cv2.imread(img_path)
except OSError:
    print("Could not read file: " + img_path)
    exit(1)

# Resize the image to decrease the resolution
height = img.shape[0]
width = img.shape[1]
height_ratio = float(height / 300)
width_ratio = float(width / 300)
if height_ratio <= width_ratio:
    width = width / width_ratio
    height = height / width_ratio
else:
    width = width / height_ratio
    height = height / height_ratio
dim = (int(width), int(height))
img_resize = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)

# Display the resized image
cv2.imshow('Original', img_resize)
cv2.waitKey(0)

# TODO: Contrast colors in image for better edge detection

# Convert to graycsale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Blur the image for better edge detection
img_blur = cv2.GaussianBlur(img_gray, (3,3), 0)
# TODO: Apply background erosion to remove unnecessary background edges

# Canny Edge Detection
edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200) # Canny Edge Detection
# Display Canny Edge Detection Image
cv2.imshow('Canny Edge Detection', edges) # edges is a binary mask of the detected edges
cv2.waitKey(0)
 
cv2.destroyAllWindows()