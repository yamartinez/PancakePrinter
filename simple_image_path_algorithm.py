#Read and process image
import cv2
import numpy as np
import math
from skimage.morphology import skeletonize
import time

IMG_SIZE = 300

start = time.time()
# Read the original image
img_path = 'baby.jpg'
img = cv2.imread(img_path)
width = img.shape[1]
height = img.shape[0]
max_dim = width if width > height else height
scale_percent = IMG_SIZE / max_dim
# Resize the image to decrease the resolution
img_resize = img # cv2.resize(img, (int(width*scale_percent), int(height*scale_percent)), interpolation=cv2.INTER_AREA)

# Display the resized image
# cv2.imshow('Original', img_resize)
# cv2.waitKey(0)

# Convert to grayscale
img_gray = cv2.cvtColor(img_resize, cv2.COLOR_BGR2GRAY)
# Blur the image for better edge detection
img_blur = cv2.GaussianBlur(img_gray, (3, 3), 0)

# Canny Edge Detection
edges = cv2.Canny(image=img_blur, threshold1=100, threshold2=200)  # Canny Edge Detection
# Display Canny Edge Detection Image
edges = cv2.resize(edges, (int(width*scale_percent), int(height*scale_percent)), interpolation=cv2.INTER_AREA)

# normalize coloring after resize
not_black_pixels = np.where((edges[:,:] != 0))
edges[not_black_pixels] = 255

#cv2.imshow('Canny Edge Detection', edges)
cv2.waitKey(0)



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

def check_surrounding_pixels(group, x, y):
    '''
    Checks the 8 pixels surrounding the (x,y) position in the provided group's grid, if they exist.
    Returns true if any of the surrounding pixels are the edge_color.
    '''
    #North
    if (y - 1) >= 0:
        if group[y-1, x] == edge_color:
            return True
    #South
    if (y + 1) <= height - 1:
        if group[y+1, x] == edge_color:
            return True
    #East
    if (x + 1) <= width - 1:
        if group[y, x+1] == edge_color:
            return True
    #West
    if (x - 1) >= 0:
        if group[y, x-1] == edge_color:
            return True
    #Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y-1, x+1] == edge_color:
            return True
    #Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if group[y-1, x-1] == edge_color:
            return True
    #Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if group[y+1, x+1] == edge_color:
            return True
    #Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if group[y+1, x-1] == edge_color:
            return True
    return False

def merge(group1, group2):
    '''
    Given two grids of identical size, creates a new grid containing all edge_color values from both inputs.
    '''
    new_grid = np.zeros((height, width), np.uint8)
    for y in range(height):
        for x in range(width):
            if group1[y,x] == edge_color or group2[y,x] == edge_color:
                new_grid[y,x] = edge_color
    return new_grid

for y in range(height):
    #print("Row", y, "of", height)
    for x in range(width):
        if current_test[y,x] == edge_color:
            #Decide whether or not this pixel belongs to another group or not
            group_indices = [] #Tracks the indices (in groupings) of the existing groups which should contain the current pixel
            for i in range(len(groupings)):
                if check_surrounding_pixels(groupings[i], x, y):
                    group_indices.append(i)

            #If no current groups contain the pixel, make a new group
            if len(group_indices) == 0:
                empty_grid = np.zeros((height, width), np.uint8)
                empty_grid[y,x] = edge_color
                groupings.append(empty_grid)
            else:
                #Add the pixel to the first group that contains it
                groupings[group_indices[0]][y,x] = edge_color

                #Merge all groups that contain the pixel
                while len(group_indices) > 1:
                    group2 = groupings[group_indices[-1]]
                    group1 = groupings[group_indices[-2]]
                    new_group = merge(group1, group2)

                    groupings[group_indices[-2]] = new_group
                    groupings.pop(group_indices[-1])
                    group_indices.pop()


# #print("Test Case:")
# for x in current_test:
#     #print(x)

#print("# of Groupings", len(groupings))
# for group in groupings:
#     #print()
#     ##print each grouping as a grid
#     for row in group:
#         #print(row)

# ##print test case as an image
# cv2.imshow('Image', edges)
# cv2.waitKey(0)

# ##print groupings as images
# for group in groupings:
#     cv2.imshow('Grouping', group)
#     cv2.waitKey(0)

# #Merge all groups into into one image for visualization purposes
# merged_grid = np.zeros((height, width, 3), np.uint8)
# colors = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (0,255,255), (255,0,255), (240,128,128), (255,215,0), (0,100,0), (32,178,170)]
# for y in range(height):
#     for x in range(width):
#         for i in range(len(groupings)):
#             if np.all(groupings[i][y,x] == edge_color):
#                 merged_grid[y,x] = colors[i%len(colors)]
# cv2.imshow('Merged Groups', merged_grid)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

#Pathing

#Calculate the "center of gravity" for each grouping
#Track grouping by index
x_centers = []
y_centers = []
pixels_per_group = []
for i in range(len(groupings)):
    group = groupings[i]
    x_centers.append(0)
    y_centers.append(0)
    pixels_per_group.append(0)
    for y in range(height):
        for x in range(width):
            if group[y,x] == edge_color:
                pixels_per_group[i] += 1
                x_centers[i] += x
                y_centers[i] += y
for i in range(len(groupings)):
    x_centers[i] = x_centers[i] / pixels_per_group[i]
    y_centers[i] = y_centers[i] / pixels_per_group[i]

#Show centers:
#print()
#print("Centers by Grouping:")
# for i in range(len(groupings)):
    #print("(x,y): (" + str(x_centers[i]) + ", " + str(y_centers[i]) + ")")

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

#Show starting pixels:
#print()
#print("Starting Pixels by Grouping:")
# for start in starting_pixels:
    #print(start)

paths = [] #3D array - list of lists (a path covering a group) of pixel coordinates

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

def pixel_group_dfs(i, x, y):
    '''
    Takes in i to track the group it's in, and the x, y values to track which pixel it's on.
    '''
    global paths
    global groupings
    #Mark the current pixel as visited and add it to the path
    groupings[i][y,x] = background_color
    paths[-1].append([x, y])

    #Recurse for all pixels adjacent to this pixel
    adjacents = make_adjacent_list(i, x, y) #List of lists (pixel coordinates - [x, y])
    for pixel in adjacents:
        ##print("pixel[1]", pixel[1])
        if groupings[i][pixel[1], pixel[0]] == edge_color:
            pixel_group_dfs(i, pixel[0], pixel[1])


# for i in range(len(groupings)):
#     group = groupings[i]
#     current = starting_pixels[i]
#     paths.append([]) #Add a new path list
#
#     #Conduct DFS on the starting pixel
#     pixel_group_dfs(i, current[0], current[1])
#
# ##print paths
# #print()
# #print("Paths by Grouping:")
# for path in paths:
#     #print(path)


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

##print paths
#print()
#print("Paths by Grouping - Iterative:")
# for path in draw_paths:
    #print(path)
end = time.time()
print("Done in",end-start)
#Export this information to a file
with open('paths.txt', 'w') as f:
    f.write(str(width) + "\n")
    f.write(str(height) + "\n")
    for path in draw_paths:
        f.write("Grouping\n")
        for pixel in path:
            f.write(str(pixel[0]) + " " + str(pixel[1]) + "\n")