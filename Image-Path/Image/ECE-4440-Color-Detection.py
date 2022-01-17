# Source: https://www.pyimagesearch.com/2014/08/04/opencv-python-color-detection/

import sys
import cv2
import numpy as np

# Define the list of colors to be detected
boundaries = [
    ([0], [101]),   # black
    ([102], [203]), # gray
    ([204], [255])  # white
]

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

"""
# NOTE: Do we need contrast? It makes the color detection more detailed and less generalized... 
# Leaving contrast here for now in case we decide we want it...
contrast = 64
alpha = 131*(contrast + 127)/(127*(131-contrast))
gamma = 127*(1-alpha)
img_contrast = cv2.addWeighted(img_resize, alpha, img_resize, 0, gamma)
"""

# Convert to graycsale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Normalize image
img_normalize = np.zeros((img_gray.shape[0], img_gray.shape[1]))
img_normalize = cv2.normalize(img_gray, img_normalize, 0, 255, cv2.NORM_MINMAX)
# Blur the image for better color detection
img_blur = cv2.GaussianBlur(img_normalize, (5,5), 0)

# TODO: Apply background erosion to remove unnecessary background colors

# TODO: Associate edges with colors by doing a Bitwise-AND between edges and colors

# loop over the boundaries
for (lower, upper) in boundaries:
	# create NumPy arrays from the boundaries
	lower = np.array(lower, dtype = "uint8")
	upper = np.array(upper, dtype = "uint8")
	# find the colors within the specified boundaries and apply
	# the mask
	mask = cv2.inRange(img_blur, lower, upper)
	# show the images
	cv2.imshow("images", np.hstack([img_blur, mask]))
	cv2.waitKey(0)

cv2.destroyAllWindows()
