import cv2
from numpy import zeros, array, where
import sys
import numpy as np
import math
from skimage.morphology import skeletonize

#Constants that can be changed by users
background_color = 0 #Black
edge_color = 255 #White

def make_skeleton(image_name):
    #Skeletonization of image to make lines 1 pixel wide
    binary_edges = np.zeros((height, width), np.uint8)
    # if debug == True:
    #     print("height:", height)
    #     print("width:", width)
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
    # cv2.imshow("Skeleton", skeleton)
    # cv2.waitKey(0)
    return skeleton

def find_surrounding_pixels(group, x, y):
    '''
    Given an (x,y) coordinate, checks the surrounding pixels in the current_test
    Erases the pixels from current_test and returns a list of colored pixels
    '''
    global current_test, background_color, edge_color
    colored_pixels = []
    # North
    if (y - 1) >= 0:
        if group[y - 1, x] == edge_color:
            colored_pixels.append([y - 1, x])
            current_test[y - 1, x] = background_color
    # South
    if (y + 1) <= height - 1:
        if group[y + 1, x] == edge_color:
            colored_pixels.append([y + 1, x])
            current_test[y + 1, x] = background_color
    # East
    if (x + 1) <= width - 1:
        if group[y, x + 1] == edge_color:
            colored_pixels.append([y, x + 1])
            current_test[y, x + 1] = background_color
    # West
    if (x - 1) >= 0:
        if group[y, x - 1] == edge_color:
            colored_pixels.append([y, x - 1])
            current_test[y, x - 1] = background_color
    # Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y - 1, x + 1] == edge_color:
            colored_pixels.append([y - 1, x + 1])
            current_test[y - 1, x + 1] = background_color
    # Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if group[y - 1, x - 1] == edge_color:
            colored_pixels.append([y - 1, x - 1])
            current_test[y - 1, x - 1] = background_color
    # Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if group[y + 1, x + 1] == edge_color:
            colored_pixels.append([y + 1, x + 1])
            current_test[y + 1, x + 1] = background_color
    # Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if group[y + 1, x - 1] == edge_color:
            colored_pixels.append([y + 1, x - 1])
            current_test[y + 1, x - 1] = background_color
    return colored_pixels

def find_groups_and_centers():
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
    return x_centers, y_centers, pixels_per_group, a_pixel_per_group

def find_starting_pixel(x_centers, y_centers):
    starting_pixels = []
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
    return starting_pixels

def make_adjacent_list(i, x, y):
    '''
    Checks the 8 pixels surrounding the (x,y) position.
    Returns a list of the [x,y] coordinates that exist adjacent to the given pixel and are edges.
    '''
    global current_test, background_color, edge_color, groupings, width, height
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

def wet_or_dry(pixel1, pixel2):
    #Determines whether to move_wet or move_dry
    #Only move_wet if the pixels are right next to each other
    if (abs(pixel1[0] - pixel2[0]) <= 1) and (abs(pixel1[1] - pixel2[1]) <= 1):
        return "move_wet"
    return "move_dry"

def find_paths_dfs(starting_pixels):
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
        draw_paths[-1].append("move_dry") #Always move_dry at the beginnning of a grouping
        draw_paths[-1].append(current)
        group[current[1]][current[0]] = background_color

        #Conduct DFS from the starting pixel
        while len(stack) != 0:

            #Move on to the next current
            current = stack.pop()
            # Mark a pixel as "visited" by adding to the path and changing it to the background color
            # Decide if the pixel should be move_dry or move_wet based on its relation to previous pixel
            draw_paths[-1].append(wet_or_dry(current, draw_paths[-1][-1]))
            draw_paths[-1].append(current)
            group[current[1]][current[0]] = background_color

            next_list = make_adjacent_list(i, current[0], current[1])
            for pixel in next_list:
                stack.append(pixel)
    return draw_paths

def optimize_paths(draw_paths):
    draw_paths_optimized = []
    for path in draw_paths:
        #control variables
        first = True
        second = False
        # initialize
        x1 = 0
        y1 = 0
        x2 = 0
        y2 = 0
        pixel1 = 0
        pixel2 = 0
        pixel1_instruction = ""
        pixel2_instruction = ""
        slope = 0
        status = "complete"
        optimized_path = []
        for i in range(1, len(path), 2):
            pixel = path[i]
            pixel_instruction = path[i-1]
            if first == True:
                x1 = pixel[0]
                y1 = pixel[1]
                pixel1 = pixel
                pixel1_instruction = pixel_instruction
                first = False
                second = True
                status = "add_one"
            elif second == True:
                x2 = pixel[0]
                y2 = pixel[1]
                pixel2 = pixel
                pixel2_instruction = pixel_instruction
                second = False
                status = "add_two"
                if x2 == x1:
                    slope = "undefined"
                else:
                    slope = (y2 - y1)/(x2 - x1)
            else:
                if pixel[0] == x1:
                    current_slope = "undefined"
                else:
                    current_slope = (pixel[1] - y1)/(pixel[0] - x1)

                if current_slope == slope:
                    x2 = pixel[0]
                    y2 = pixel[1]
                    pixel2 = pixel
                    pixel2_instruction = pixel_instruction
                else:
                    optimized_path.append(pixel1_instruction)
                    optimized_path.append(pixel1)
                    optimized_path.append(pixel2_instruction)
                    optimized_path.append(pixel2)
                    x1 = pixel[0]
                    y1 = pixel[1]
                    pixel1 = pixel
                    pixel1_instruction = pixel_instruction
                    second = True
                    status = "complete"
        if status == "add_one":
            optimized_path.append(pixel1_instruction)
            optimized_path.append(pixel1)
        elif status == "add_two":
            optimized_path.append(pixel1_instruction)
            optimized_path.append(pixel1)
            optimized_path.append(pixel2_instruction)
            optimized_path.append(pixel2)
        draw_paths_optimized.append(optimized_path)
    return draw_paths_optimized

def add_waits(draw_paths_optimized):
    for path in draw_paths_optimized:
        path.append("end")
        prev_instruction = "move_dry"
        i = 0
        while path[i] != "end":
            pixel = path[i]
            if pixel == "move_dry" or pixel == "move_wet":
                if pixel != prev_instruction:
                    path.insert(i, "wait 1")
                    i += 2
                    prev_instruction = pixel
                else:
                    i += 1
            else:
                i += 1
        path.remove("end")
        path.insert(0, "wait 1")
    return draw_paths_optimized

def debug_print(draw_paths_optimized):
    total = 0
    for path in draw_paths_optimized:
        for pixel in path:
            if pixel == "move_dry" or pixel == "move_wet":
                print(pixel, end = " ")
            elif pixel == "wait 1":
                print(pixel, end = "\n")
                total += 1
            else:
                print(pixel[0], pixel[1])
                total += 1
    print("Total instructions:", total)

def write_to_file(filename, debug, draw_paths_optimized):
    with open(filename, 'w') as f:
        if debug:
            f.write(str(width) + "\n")
            f.write(str(height) + "\n")
        for path in draw_paths_optimized:
            for i in range(len(path)):
                pixel = path[i]
                if pixel == "move_dry" or pixel == "move_wet":
                    f.write(pixel + " ")
                elif pixel == "wait 1":
                    f.write(pixel + "\n")
                else:
                    f.write(str(pixel[0]) + " " + str(pixel[1]) + "\n")

def find_coordinates(image, filename, debug = False):
    #Establish globals to share with other functions
    global current_test, background_color, edge_color, groupings, width, height

    #List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
    groupings = []
    height, width = image.shape

    current_test = make_skeleton(image)

    #Find each group and track the cumulative placement of pixels in each group
    x_centers, y_centers, pixels_per_group, a_pixel_per_group = find_groups_and_centers()

    #Pathing
    #Calculate the "center of gravity" for each grouping
    for i in range(len(groupings)):
        x_centers[i] = x_centers[i] / pixels_per_group[i]
        y_centers[i] = y_centers[i] / pixels_per_group[i]

    #THIS CAN BE OPTIMIZED vvv
    #Find one pixel per grouping which is the farthest from the center of gravity
    #This pixel will serve as the starting point for the path decision
    starting_pixels = find_starting_pixel(x_centers, y_centers) #2D array of starting pixels - [x,y]

    #For each grouping, start with the selected "begin" pixel, then search from there
    draw_paths = find_paths_dfs(starting_pixels) #3D array - list of lists (a path covering a group) of pixel coordinates

    #Removes unnecessary in-between pixels which follow the same slope
    draw_paths_optimized = optimize_paths(draw_paths)

    #Add in waits
    draw_paths_optimized = add_waits(draw_paths_optimized)

    if debug == True:
        debug_print(draw_paths_optimized)

    #Export this information to a file
    write_to_file(filename, debug, draw_paths_optimized)

# Returns edges as binary mask and colors as list of binary masks
def processImage(img, show=False, dilation=1):
    # ------------------- EDGE DETECTION -------------------

    # Check if image is valid
    if (img is None):
        print("Could not read image")
        exit(1)

    # Using cv2.rotate() method
    # Using cv2.ROTATE_90_CLOCKWISE rotate
    # by 90 degrees clockwise
    img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)

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

    improved_edges += edges_resized
    improved_edges = cv2.morphologyEx(improved_edges, cv2.MORPH_DILATE, kernel)

    if show:
        cv2.imshow("dilated", edges_morph)
        cv2.waitKey(0)
    return edges_morph, color_masks

if(__name__ == "__main__"):
    try:
        name = sys.argv[1]
        filename = sys.argv[2]
    except:
        print("Please provide the correct arguments")
        exit(1)
    img = cv2.imread(name)
    edges_morph, color_masks = processImage(img)
    find_coordinates(edges_morph, filename)