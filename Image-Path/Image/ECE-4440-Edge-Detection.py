# Sources: https://learnopencv.com/edge-detection-using-opencv/
#          https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/

import sys
import cv2
import numpy as np

# Read the original image
name = sys.argv[1]
img_path = 'Edge-Detection-Test-' + name + '.jpg'
img = cv2.imread(img_path)
if (img is None):
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

"""
# NOTE: Do we need contrast? It does improve the edge detection for more complex images... 
# Leaving contrast here for now in case we decide we want it...
contrast = 64
alpha = 131*(contrast + 127)/(127*(131-contrast))
gamma = 127*(1-alpha)
img_contrast = cv2.addWeighted(img_resize, alpha, img_resize, 0, gamma)

# TESTING
cv2.imshow('Contrast', img_contrast)
cv2.waitKey(0)
"""

# Convert to graycsale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Normalize image
img_normalize = np.zeros(img_gray.shape[:2])
img_normalize = cv2.normalize(img_gray, img_normalize, 0, 255, cv2.NORM_MINMAX)

# Blur the image for better edge detection
img_blur = cv2.GaussianBlur(img_normalize, (3,3), 0)

# Canny Edge Detection
edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200) # Canny Edge Detection
# Display Canny Edge Detection Image
cv2.imshow('Canny Edge Detection', edges) # edges is a binary mask of the detected edges
cv2.waitKey(0)

# Dilate image to connect loose ends of edges
kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3,3))
img_morph = cv2.morphologyEx(edges, cv2.MORPH_DILATE, kernel)
cv2.imshow('With Morphological Transformation', img_morph)
cv2.waitKey(0)

"""
# Background erosion is a work in progress, keeping it in case we want it...
# IDEA: Don't find contours, just use edges to find contour area?
img_contours = cv2.findContours(img_morph, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)[-2]
img_contours = sorted(img_contours, key=cv2.contourArea)
for i in img_contours:
    if cv2.contourArea(i) > 9000:
        break
mask = np.zeros(img_morph.shape[:2], np.uint8)
cv2.drawContours(mask, [i], -1, 255, -1)
cv2.imshow('Contours', mask)
cv2.waitKey(0)
edges = cv2.bitwise_and(edges, mask)
cv2.imshow('Background Erosion', edges) # edges is a binary mask of the detected edges
cv2.waitKey(0)
"""

cv2.destroyAllWindows()