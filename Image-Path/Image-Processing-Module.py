import cv2
from numpy import zeros, array, where
import sys

# Returns edges as binary mask and colors as list of binary masks
def processImage(img, show=False):
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
    img_res = 150
    max_dim = height if height >= width else width
    scale_factor = float(max_dim / img_res)
    height /= scale_factor
    width /= scale_factor
    dim = (int(width), int(height))
    edges_resized = cv2.resize(edges, dim, interpolation = cv2.INTER_AREA)
    not_black_pixels = where((edges_resized[:,:] != 0))
    edges_resized[not_black_pixels] = 255

    # Dilate image to connect loose ends of edges
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3,3))
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

    improved_edges += edges_resized
    improved_edges = cv2.morphologyEx(improved_edges, cv2.MORPH_DILATE, kernel)

    return edges_morph, color_masks

if(__name__ == "__main__"):
    name = sys.argv[1]
    img = cv2.imread(name)
    processImage(img, True)