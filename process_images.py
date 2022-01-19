#!/usr/bin/python3
import cv2
from numpy import zeros, array, where
import sys
from skimage.morphology import skeletonize
import numpy as np
from pathing_module import find_coordinates

# Constants that can be changed by users
background_color = 0  # Black
edge_color = 255  # White

def make_skeleton(image_name, debug=False):
    height, width = image_name.shape
    # Skeletonization of image to make lines 1 pixel wide
    binary_edges = np.zeros((height, width), np.uint8)
    for y in range(height):
        for x in range(width):
            if image_name[y, x] == edge_color:
                binary_edges[y, x] = 1
    skeleton_binary = skeletonize(binary_edges)
    skeleton = np.zeros((height, width), np.uint8)
    for y in range(height):
        for x in range(width):
            if skeleton_binary[y, x] == 1:
                skeleton[y, x] = edge_color
    if debug:
        cv2.imshow("skeleton", skeleton)
        cv2.waitKey(0)
    return skeleton

# Returns edges as binary mask and colors as list of binary masks
def processImage(img, show=False, dilation=5):
    # ------------------- EDGE DETECTION -------------------

    # Check if image is valid
    if (img is None):
        print("Could not read image")
        exit(1)

    # Convert to graycsale
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Normalize image
    img_normalize = zeros(img_gray.shape[:2])
    img_normalize = cv2.normalize(img_gray, img_normalize, 0, 255, cv2.NORM_MINMAX)

    # Blur the image for better edge detection
    img_blur = cv2.GaussianBlur(img_normalize, (3,3), 0)

    # Canny Edge Detection
    edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200) # Canny Edge Detection

    # Resize the edges to decrease the resolution
    height = edges.shape[0]
    width = edges.shape[1]
    img_res = 100
    max_dim = height if height >= width else width
    scale_factor = float(max_dim / img_res)
    height /= scale_factor
    width /= scale_factor
    dim = (int(width), int(height))
    edges_resized = cv2.resize(edges, dim, interpolation = cv2.INTER_AREA)
    not_black_pixels = where((edges_resized[:,:] != 0))
    edges_resized[not_black_pixels] = 255

    # Dilate image to connect loose ends of edges
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (dilation, dilation))
    edges_morph = cv2.morphologyEx(edges_resized, cv2.MORPH_DILATE, kernel)

    if show:
        cv2.imshow('Edges', edges_morph)
        cv2.waitKey(0)

    # ------------------- COLOR DETECTION -------------------

    # Define the list of colors to be detected
    boundaries = [
        ([0], [101]),   # black
        ([102], [203]), # gray
        ([204], [255])  # white
    ]

    # Resize the image to decrease the resolution
    img_resize = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
    # Convert to graycsale
    img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
    # Normalize image
    img_normalize = zeros(img_gray.shape[:2])
    img_normalize = cv2.normalize(img_gray, img_normalize, 0, 255, cv2.NORM_MINMAX)
    # Blur the image for better color detection
    img_blur = cv2.GaussianBlur(img_normalize, (5,5), 0)

    color_masks = list()
    improved_edges = zeros(img_blur.shape[:2])
    # loop over the boundaries
    for (lower, upper) in boundaries:
        # create NumPy arrays from the boundaries
        lower = array(lower, dtype = "uint8")
        upper = array(upper, dtype = "uint8")
        # find the colors within the specified boundaries and apply
        # the mask
        mask = cv2.inRange(img_blur, lower, upper)
        color_masks.append(mask)
        # Find edges that match this color
        colorEdges = cv2.bitwise_and(edges_morph, mask)
        colorEdges = cv2.morphologyEx(colorEdges, cv2.MORPH_DILATE, kernel)
        # TESTING
        new_edges = cv2.Canny(mask, threshold1=100, threshold2=200)
        improved_edges += new_edges

    improved_edges += edges_morph
    improved_edges = cv2.morphologyEx(improved_edges, cv2.MORPH_DILATE, kernel)

    skeleton = make_skeleton(edges_morph, False)
    return skeleton

if(__name__ == "__main__"):
    name = sys.argv[1]
    img = cv2.imread(name)
    final5 = processImage(img, False, 1)
    final4 = processImage(img, False, 3)
    final3 = processImage(img, False, 4)
    final2 = processImage(img, False, 6)
    final1 = processImage(img, False, 7)
    
    find_coordinates(final1,sys.argv[7])
    find_coordinates(final2,sys.argv[8])
    find_coordinates(final3,sys.argv[9])
    find_coordinates(final4,sys.argv[10])
    find_coordinates(final5,sys.argv[11])

    final1 = cv2.bitwise_not(final1)
    cv2.imwrite(sys.argv[2],final1)
    final2 = cv2.bitwise_not(final2)
    cv2.imwrite(sys.argv[3],final2)
    final3 = cv2.bitwise_not(final3)
    cv2.imwrite(sys.argv[4],final3)
    final4 = cv2.bitwise_not(final4)
    cv2.imwrite(sys.argv[5], final4)
    final5 = cv2.bitwise_not(final5)
    cv2.imwrite(sys.argv[6], final5)
