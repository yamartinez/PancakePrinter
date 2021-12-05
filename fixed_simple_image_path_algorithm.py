#Read and process image
import cv2
import numpy as np
import math
from skimage.morphology import skeletonize

# Read the original image
img_path = 'Edge-Detection-Test-Medium.jpg'
img = cv2.imread(img_path)
# Resize the image to decrease the resolution
img_resize = img
scale_percent = 80  # percent of original size
width = int(img.shape[1] * scale_percent / 100)
height = int(img.shape[0] * scale_percent / 100)
dim = (width, height)
while (width > 300 or height > 300):
    img_resize = cv2.resize(img_resize, dim, interpolation=cv2.INTER_AREA)
    width = int(img_resize.shape[1] * scale_percent / 100)
    height = int(img_resize.shape[0] * scale_percent / 100)
    dim = (width, height)

# Convert to grayscale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Blur the image for better edge detection
img_blur = cv2.GaussianBlur(img_gray, (3, 3), 0)

# Canny Edge Detection
edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200)  # Canny Edge Detection


#List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
groupings = []
height, width = edges.shape
background_color = 0
edge_color = 255

# #Skeletonization
binary_edges = np.zeros((height, width), np.uint8)
for y in range(height):
    for x in range(width):
        if edges[y, x] == edge_color:
            binary_edges[y, x] = 1
skeleton_binary = skeletonize(binary_edges)
skeleton = np.zeros((height, width), np.uint8)
for y in range(height):
    for x in range(width):
        if skeleton_binary[y, x] == 1:
            skeleton[y, x] = edge_color

current_test = skeleton

def find_surrounding_pixels(group, x,y):
    '''
    Given an (x,y) coordinate, checks the surrounding pixels in the current_test
    Erases the pixels from current_test and returns a list of colored pixels
    '''
    global current_test
    colored_pixels = []
    #North
    if (y - 1) >= 0:
        if group[y-1, x] == edge_color:
            colored_pixels.append([y-1, x])
            current_test[y-1, x] = background_color
    #South
    if (y + 1) <= height - 1:
        if group[y+1, x] == edge_color:
            colored_pixels.append([y+1, x])
            current_test[y+1, x] = background_color
    #East
    if (x + 1) <= width - 1:
        if group[y, x+1] == edge_color:
            colored_pixels.append([y, x+1])
            current_test[y, x+1] = background_color
    #West
    if (x - 1) >= 0:
        if group[y, x-1] == edge_color:
            colored_pixels.append([y, x-1])
            current_test[y, x-1] = background_color
    #Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y-1, x+1] == edge_color:
            colored_pixels.append([y-1, x+1])
            current_test[y-1, x+1] = background_color
    #Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if group[y-1, x-1] == edge_color:
            colored_pixels.append([y-1, x-1])
            current_test[y-1, x-1] = background_color
    #Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if group[y+1, x+1] == edge_color:
            colored_pixels.append([y+1, x+1])
            current_test[y+1, x+1] = background_color
    #Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if group[y+1, x-1] == edge_color:
            colored_pixels.append([y+1, x-1])
            current_test[y+1, x-1] = background_color
    return colored_pixels

#Find each group and track the cumulative placement of pixels in each group
x_centers = []
y_centers = []
pixels_per_group = []
a_pixel_per_group = [] #[x,y] format
for y in range(height):
    for x in range(width):
        if current_test[y,x] == edge_color:
            #If edge color is found, make new grouping
            empty_grid = np.zeros((height, width), np.uint8)
            empty_grid[y, x] = edge_color
            x_sum = 0
            y_sum = 0
            num_pixels = 0
            a_pixel_per_group.append([x,y])

            #Find and add the rest of the connected pixels to the grouping
            to_add = find_surrounding_pixels(current_test, x, y)
            while len(to_add) != 0:
                pixel_to_add = to_add[0]
                empty_grid[pixel_to_add[0], pixel_to_add[1]] = edge_color
                x_sum += pixel_to_add[1]
                y_sum += pixel_to_add[0]
                num_pixels += 1
                to_add += find_surrounding_pixels(current_test, pixel_to_add[1], pixel_to_add[0])
                to_add.pop(0)

            groupings.append(empty_grid)
            x_centers.append(x_sum)
            y_centers.append(y_sum)
            pixels_per_group.append(num_pixels)


#Pathing

#Calculate the "center of gravity" for each grouping
for i in range(len(groupings)):
    x_centers[i] = x_centers[i] / pixels_per_group[i]
    y_centers[i] = y_centers[i] / pixels_per_group[i]

#THIS CAN BE OPTIMIZED vvv
#Find one pixel per grouping which is the farthest from the center of gravity
#This pixel will serve as the starting point for the path decision
starting_pixels = [] #2D array of starting pixels - [x,y]
for i in range(len(groupings)):
    group = groupings[i]
    starting_pixels.append([0,0])
    max_distance = -1
    for y in range(height):
        for x in range(width):
            if group[y,x] == edge_color:
                #When you find a pixel in a group, evaluate its distance from the center of gravity
                distance = math.sqrt((y - y_centers[i])**2 + (x - x_centers[i])**2)
                if distance > max_distance:
                    max_distance = distance
                    starting_pixels[i] = [x, y]

def make_adjacent_list(i, x, y):
    '''
    Checks the 8 pixels surrounding the (x,y) position.
    Returns a list of the [x,y] coordinates that exist adjacent to the given pixel and are edges.
    '''
    adjacents = []
    #North
    if (y - 1) >= 0:
        if groupings[i][y-1][x] == edge_color:
            adjacents.append([x, y-1])
    #South
    if (y + 1) <= height - 1:
        if groupings[i][y+1][x] == edge_color:
            adjacents.append([x, y+1])
    #East
    if (x + 1) <= width - 1:
        if groupings[i][y][x+1] == edge_color:
            adjacents.append([x+1, y])
    #West
    if (x - 1) >= 0:
        if groupings[i][y][x-1] == edge_color:
            adjacents.append([x-1, y])
    #Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if groupings[i][y-1][x+1] == edge_color:
            adjacents.append([x+1, y-1])
    #Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if groupings[i][y-1][x-1] == edge_color:
            adjacents.append([x-1, y-1])
    #Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if groupings[i][y+1][x+1] == edge_color:
            adjacents.append([x+1, y+1])
    #Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if groupings[i][y+1][x-1] == edge_color:
            adjacents.append([x-1, y+1])
    return adjacents


paths = [] #3D array - list of lists (a path covering a group) of pixel coordinates

#Iterative Approach - DFS
#For each grouping, start with the selected "begin" pixel, then search from there
draw_paths = []
for i in range(len(groupings)):
    group = groupings[i]
    draw_paths.append([]) #Add a new path list
    stack = [] #Stores pixels to consider next

    current = starting_pixels[i]  #Of the form [x,y]

    next_list = make_adjacent_list(i, current[0], current[1])

    for pixel in next_list:
        stack.append(pixel)

    # Mark a pixel as "visited" by adding to the path and changing it to the background color
    draw_paths[-1].append(current)
    group[current[1]][current[0]] = background_color

    #Conduct DFS from the starting pixel
    while len(stack) != 0:

        #Move on to the next current
        current = stack.pop()
        # Mark a pixel as "visited" by adding to the path and changing it to the background color
        draw_paths[-1].append(current)
        group[current[1]][current[0]] = background_color

        next_list = make_adjacent_list(i, current[0], current[1])
        for pixel in next_list:
            stack.append(pixel)

#Export this information to a file
with open('paths.txt', 'w') as f:
    f.write(str(width) + "\n")
    f.write(str(height) + "\n")
    for path in draw_paths:
        f.write("Grouping\n")
        for pixel in path:
            f.write(str(pixel[0]) + " " + str(pixel[1]) + "\n")