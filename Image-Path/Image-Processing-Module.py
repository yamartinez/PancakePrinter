import cv2
from sys import argv
from numpy import zeros, array, hstack

# EDGE DETECTION

# Read the original image
name = argv[1]
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

# Convert to graycsale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Normalize image
img_normalize = zeros(img_gray.shape[:2])
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
edges = cv2.morphologyEx(edges, cv2.MORPH_DILATE, kernel)
cv2.imshow('With Morphological Transformation', edges)
cv2.waitKey(0)

# COLOR DETECTION

# Define the list of colors to be detected
boundaries = [
    ([0], [101]),   # black
    ([102], [203]), # gray
    ([204], [255])  # white
]

img_blur = cv2.GaussianBlur(img_normalize, (5,5), 0)

# TODO: Apply background erosion to remove unnecessary background colors

# loop over the boundaries
for (lower, upper) in boundaries:
    # create NumPy arrays from the boundaries
    lower = array(lower, dtype = "uint8")
    upper = array(upper, dtype = "uint8")
    # find the colors within the specified boundaries and apply
    # the mask
    mask = cv2.inRange(img_blur, lower, upper)
    # show the images
    cv2.imshow("images", hstack([img_blur, mask]))
    cv2.waitKey(0)
    # Find edges that match this color
    colorEdges = cv2.bitwise_and(edges, mask)
    colorEdges = cv2.morphologyEx(colorEdges, cv2.MORPH_DILATE, kernel)
    cv2.imshow("Corresponding Edges", colorEdges)
    cv2.waitKey(0)

cv2.destroyAllWindows()