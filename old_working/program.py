#!/usr/bin/python3 

import sys
import cv2
import numpy as np
import math
from skimage.morphology import skeletonize
import serial
import time

if (len(sys.argv) != 3):
    print("Usage: program <image path>")
    exit(1)

name = sys.argv[1]
img_path = name
img = cv2.imread(img_path)
if (img is None):
    print("Could not read file: " + img_path)
    exit(1)

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
    img_normalize = np.zeros(img_gray.shape[:2])
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
    not_black_pixels = np.where((edges_resized[:,:] != 0))
    edges_resized[not_black_pixels] = 255

    if show:
        cv2.imshow('Edges', edges_resized)
        cv2.waitKey(0)
   
    return edges_resized

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
    return skeleton

def find_surrounding_pixels(group, x, y, color_num, all_pixels):
    '''
    Given an (x,y) coordinate, checks the surrounding pixels in the current_test
    Changes the pixel to the color_num
    Returns a list of edge-colored pixels (to check for neighbors in the future)
    '''
    global current_test, edge_color
    colored_pixels = []
    # North
    if (y - 1) >= 0:
        if group[y - 1, x] == edge_color:
            colored_pixels.append([y - 1, x])
            current_test[y - 1, x] = color_num
            all_pixels.append([x, y-1])

    # South
    if (y + 1) <= height - 1:
        if group[y + 1, x] == edge_color:
            colored_pixels.append([y + 1, x])
            current_test[y + 1, x] = color_num
            all_pixels.append([x, y+1])
    # East
    if (x + 1) <= width - 1:
        if group[y, x + 1] == edge_color:
            colored_pixels.append([y, x + 1])
            current_test[y, x + 1] = color_num
            all_pixels.append([x+1, y])
    # West
    if (x - 1) >= 0:
        if group[y, x - 1] == edge_color:
            colored_pixels.append([y, x - 1])
            current_test[y, x - 1] = color_num
            all_pixels.append([x-1, y])
    # Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y - 1, x + 1] == edge_color:
            colored_pixels.append([y - 1, x + 1])
            current_test[y - 1, x + 1] = color_num
            all_pixels.append([x+1, y-1])
    # Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if group[y - 1, x - 1] == edge_color:
            colored_pixels.append([y - 1, x - 1])
            current_test[y - 1, x - 1] = color_num
            all_pixels.append([x - 1, y - 1])
    # Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if group[y + 1, x + 1] == edge_color:
            colored_pixels.append([y + 1, x + 1])
            current_test[y + 1, x + 1] = color_num
            all_pixels.append([x + 1, y + 1])
    # Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if group[y + 1, x - 1] == edge_color:
            colored_pixels.append([y + 1, x - 1])
            current_test[y + 1, x - 1] = color_num
            all_pixels.append([x - 1, y + 1])
    return colored_pixels, all_pixels

def find_surrounding_pixel_list(grid, x, y):
    '''
    Given an (x,y) coordinate, checks the surrounding pixels in the given grid
    Erases the pixels from the grid
    Returns a list of colored pixels and the updated grid
    '''
    global width, height, background_color
    colored_pixels = []
    # North
    if (y - 1) >= 0:
        if grid[y - 1, x] > 0:
            colored_pixels.append([x, y-1])
            grid[y - 1, x] = background_color
    # South
    if (y + 1) <= height - 1:
        if grid[y + 1, x] > 0:
            colored_pixels.append([x, y+1])
            grid[y + 1, x] = background_color
    # East
    if (x + 1) <= width - 1:
        if grid[y, x + 1] > 0:
            colored_pixels.append([x + 1, y])
            grid[y, x + 1] = background_color
    # West
    if (x - 1) >= 0:
        if grid[y, x - 1] > 0:
            colored_pixels.append([x - 1, y])
            grid[y, x - 1] = background_color
    # Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if grid[y - 1, x + 1]> 0:
            colored_pixels.append([x + 1, y-1])
            grid[y - 1, x + 1] = background_color
    # Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if grid[y - 1, x - 1] > 0:
            colored_pixels.append([x - 1, y-1])
            grid[y - 1, x - 1] = background_color
    # Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if grid[y + 1, x + 1] > 0:
            colored_pixels.append([x + 1, y+1])
            grid[y + 1, x + 1] = background_color
    # Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if grid[y + 1, x - 1] > 0:
            colored_pixels.append([x - 1, y+1])
            grid[y + 1, x - 1] = background_color
    return colored_pixels, grid

def find_groups_and_centers():
    #Dry Edge = 255, Group 0 = 254, Group 1 = 253, ... Group N = 255 - (N+1)
    #For now, assume less than 2
        #in the future, account for this - check to make sure you can go above 255 first
    x_centers = []
    y_centers = []
    pixels_per_group = []
    a_pixel_per_group = [] #[x,y] format
    all_pixels_per_group = [] #Used as a tool later
    group_num = 0 #Initialize group numbers at 0
    for y in range(height):
        for x in range(width):
            if current_test[y,x] == edge_color:
                #If edge color is found, make new grouping
                # empty_grid = np.zeros((height, width), np.uint8)
                # empty_grid[y, x] = edge_color

                #If a pixel is found, set up to track info
                x_sum = 0
                y_sum = 0
                num_pixels = 0
                a_pixel_per_group.append([x,y])
                all_pixels = [[x,y]]

                # Convert found pixel to match the group's color
                color_num = 255 - (group_num + 1)
                current_test[y, x] = color_num

                #Find and convert the rest of the pixels to the color_num
                to_add, all_pixels = find_surrounding_pixels(current_test, x, y, color_num, all_pixels)
                while len(to_add) != 0:
                    pixel_to_add = to_add[0]
                    # empty_grid[pixel_to_add[0], pixel_to_add[1]] = edge_color
                    x_sum += pixel_to_add[1]
                    y_sum += pixel_to_add[0]
                    num_pixels += 1
                    ret_to_add, all_pixels = find_surrounding_pixels(current_test, pixel_to_add[1], pixel_to_add[0], color_num, all_pixels)
                    to_add += ret_to_add
                    to_add.pop(0)

                # groupings.append(empty_grid)
                x_centers.append(x_sum)
                y_centers.append(y_sum)
                pixels_per_group.append(num_pixels)
                all_pixels_per_group.append(all_pixels)
                group_num += 1 #Set this up to be ready for the next group
                               #Also, at any point in time, group_num = the number of completed groups
    return x_centers, y_centers, pixels_per_group, a_pixel_per_group, all_pixels_per_group

def find_distance(pixel1, pixel2):
    x1 = pixel1[0]
    y1 = pixel1[1]
    x2 = pixel2[0]
    y2 = pixel2[1]
    return math.sqrt((y2 - y1)**2 + (x2 - x1)**2)

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
    for i in range(1, len(draw_paths), 2):
        pixel = draw_paths[i]
        # print(pixel)
        pixel_instruction = draw_paths[i-1]
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
    return optimized_path

def add_waits(path):
    path.append("end")
    prev_instruction = "move_dry"
    i = 0
    while path[i] != "end":
        pixel = path[i]
        if pixel == "move_dry" or pixel == "move_wet":
            if pixel != prev_instruction:
                path.insert(i, "wait 5000")
                i += 2
                prev_instruction = pixel
            else:
                i += 1
        else:
            i += 1
    path.remove("end")
    path.insert(0, "wait 5000")
    return path

def debug_print(draw_paths_optimized):
    total = 0
    for path in draw_paths_optimized:
        for pixel in path:
            if pixel == "move_dry" or pixel == "move_wet" or pixel == "wait 5000":
                print(pixel, end = " ")
            else:
                print(pixel[0], pixel[1])
                total += 1
    print("Total instructions:", total)

def write_to_file(filename, debug, full_path):
    with open(filename, 'w') as f:
        if debug:
            f.write(str(width) + "\n")
            f.write(str(height) + "\n")
        for pixel in full_path:
            if pixel == "move_dry" or pixel == "move_wet":
                f.write(pixel + " ")
            elif pixel == "wait 5000":
                f.write(pixel + "\n")
            else:
                f.write(str(pixel[0]) + " " + str(pixel[1]) + "\n")

def check_circle(center, radius, grid):
    '''
    Checks the circle around the given center pixel with the given radius
    Checks for a non-zero pixel within the given grid

    Returns 0 if nothing is found
    Returns the pixel in the form [x,y] if found (returns the first one found)
    '''
    global width, height
    center_x = center[0]
    center_y = center[1]
    lower_x = center_x - radius
    upper_x = center_x + radius
    lower_y = center_y - radius
    upper_y = center_y + radius

    #Check outer edges of the square made by the bounds
    #Top and bottom edges of square
    for x in range(lower_x, upper_x + 1):
        #Check lower y
        y = lower_y
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x,y]
        # Check upper y
        y = upper_y
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x, y]

    #Left and right edges of the square
    for y in range(lower_y+1, upper_y):
        #Check lower x
        x = lower_x
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x,y]
        # Check upper x
        x = upper_x
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x, y]
    return 0

def make_target_group(target_color, grid):
    global width, height
    target_group = np.zeros((height, width), np.uint8)
    num = 0
    #Populate with target-colored pixels unvisited in updated
    for x in range(width):
        for y in range(height):
            if grid[y, x] == target_color:
                target_group[y, x] = target_color
                num += 1
    # print(num, target_color)
    return target_group

def find_balanced_path(updated, current_test, start):
    '''
    Updated can be kept up to date with visited
    Current_test is for reference on grouping structure
        #Pixel groupings are represented by different numbers in current_test
        #Group 0 = 254, Group 1 = 253, ..., Group N = 255 - (N+1)
    Returns the lists of commands that makes the full balanced path
    '''
    current = start
    full_path = ["move_dry", current]  # 2D list of instructions and pixels to build the path
    updated[current[1], current[0]] = 0
    next = check_circle(current, 1, updated)
    while True:
        #Go in one direction until you can't
        while next != 0:
            current = next
            full_path.append("move_wet")
            full_path.append(current)
            updated[current[1], current[0]] = 0
            next = check_circle(current, 1, updated)
        #Reached a dead end - sonar search to find a "target point"
            #If you cannot find a target point, traversal is finished
        radius = 1
        target_point = 0
        while radius < max(width, height):
            target_point = check_circle(current, radius, updated)
            if target_point != 0:
                break
            radius += 1
        if target_point == 0:
            return full_path

        #Construct a target_group
        target_color = updated[target_point[1], target_point[0]]
        target_group = make_target_group(target_color, updated)

        #Construct a current_group to allow for updating in dfs
        current_color = current_test[current[1], current[0]]
        current_group = make_target_group(current_color, current_test)

        #Consider every pixel in current's group (in current_test)
        #Calculate a backtrack/jump/advance index for each pixel
            #Backtrack: +1
            #Advance: -1
            #Jump: +jump_constant (4?)
        #Preserve all associated: best pixel, path_to_pixel, lowest index, and jump_point
        jump_constant = 10 #How many times worse is a jump?

        dfs_current = current
        current_group[dfs_current[1], dfs_current[0]] = 0

        best_pixel = dfs_current
        min_index = find_distance(dfs_current, target_point) * jump_constant
        best_path = []
        best_jump_point = target_point

        path_to_pixel = [dfs_current] #initialize so we can judge distance from current
        list_score = 0
        jump_point = target_point
        #
        # #DFS from current point in current_group to find the best backtrack/advance/jump index
        # to_explore, current_group = find_surrounding_pixel_list(current_group, dfs_current[0], dfs_current[1])
        # while len(to_explore) > 0:
        #     dfs_current = to_explore.pop()
        #     #Adjust path_list as necessary to match up with current
        #     while (find_distance(dfs_current, path_to_pixel[-1]) >= 2):
        #         popped = path_to_pixel.pop()
        #         #Adjust list_score
        #         if updated[popped[1], popped[0]] == 0: #Popped was already visited
        #             list_score -= 1
        #         else:
        #             list_score += 1
        #     #Add current to path and adjust list_score
        #     path_to_pixel.append(dfs_current)
        #     if updated[dfs_current[1], dfs_current[0]] == 0: #dfs_current is visited = backtrack = +1
        #         list_score += 1
        #     else:
        #         list_score -= 1
        #
        #     #Find the corresponding jump_point
        #     radius = 1
        #     while radius < max(width, height):
        #         jump_point = check_circle(dfs_current, radius, target_group)
        #         if jump_point != 0:
        #             break
        #         radius += 1
        #
        #     #Calculate current index
        #     index = list_score + (jump_constant * find_distance(dfs_current, jump_point))
        #
        #     if index < min_index:
        #         min_index = index
        #         best_path = path_to_pixel[1:]
        #         best_pixel = dfs_current
        #         best_jump_point = jump_point
        #
        #     new_to_explore, current_group = find_surrounding_pixel_list(current_group, dfs_current[0], dfs_current[1])
        #     to_explore += new_to_explore

        #We are left with the best_pixel, best_path, best_jump_point
        #Traverse the path to pre-jump
        for pixel in best_path:
            if updated[pixel[1], pixel[0]] == 0: #already visited
                full_path.append("move_dry")
            else:
                full_path.append("move_wet")
            full_path.append(pixel)
            updated[pixel[1], pixel[0]] = 0

        #Dry jump
        full_path.append("move_dry")
        full_path.append(jump_point)
        updated[jump_point[1], jump_point[0]] = 0

        if jump_point != target_point:
            #DFS to find a path in target_group containing target_point
                #Stop once you find target_point
            target_path = [jump_point]
            dfs_current = jump_point
            to_explore, target_group = find_surrounding_pixel_list(target_group, dfs_current[0], dfs_current[1])
            while len(to_explore) > 0:
                dfs_current = to_explore.pop()

                # Adjust target_path if necessary before adding it to path
                while (find_distance(dfs_current, target_path[-1]) >= 2):
                    target_path.pop()
                target_path.append(dfs_current)

                #Stop once you find target point
                if dfs_current == target_point:
                    break

                new_to_explore, target_group = find_surrounding_pixel_list(target_group, dfs_current[0], dfs_current[1])
                to_explore += new_to_explore

            #Add target_path to full_path - remember that it begins with jump_point (already added)
            for pixel in target_path[1:]:
                full_path.append("move_wet")
                full_path.append(pixel)
                updated[pixel[1], pixel[0]] = 0

        #Set up for the next cycle
        current = target_point
        next = check_circle(current, 1, updated)

def find_coordinates(image, filename, debug = False):
    #Establish globals to share with other functions
    global current_test, background_color, edge_color, groupings, width, height

    #List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
    groupings = []
    height, width = image.shape

    current_test = make_skeleton(image)

    #Find each group and track the cumulative placement of pixels in each group
    x_centers, y_centers, pixels_per_group, a_pixel_per_group, all_pixels_per_group = find_groups_and_centers()

    # for y in range(height):
    #     for x in range(width):
    #         print(current_test[y, x], end = "")
    #     print()

    #Pathing
    #Calculate the "center of gravity" for each grouping
    for i in range(len(groupings)):
        x_centers[i] = x_centers[i] / pixels_per_group[i]
        y_centers[i] = y_centers[i] / pixels_per_group[i]

    #Find the starting pixel for the entire traversal
        #which is the pixel in group 0 farthest from the center of gravity
    group0 = all_pixels_per_group[0]
    true_start = group0[0]
    max_distance = -1
    for pixel in group0:
        distance = math.sqrt((pixel[1] - y_centers[0]) ** 2 + (pixel[0] - x_centers[0]) ** 2)
        if distance > max_distance:
            max_distance = distance
            true_start = pixel

    #Pathing
    updated = np.zeros((height, width), np.uint8)
    for y in range(height):
        for x in range(width):
            if current_test[y, x] > 0:
                number = current_test[y, x]
                updated[y, x] = number
    # updated = current_test
    full_path = find_balanced_path(updated, current_test, true_start)
    # print(full_path)
    # print(len(full_path)/2)
    # for i in range(1, len(full_path), 2):
    #     print(full_path[i])

    #Removes unnecessary in-between pixels which follow the same slope
    full_path = optimize_paths(full_path)

    #Add in waits
    full_path = add_waits(full_path)

    # if debug == True:
    #     debug_print(full_path)

    #Export this information to a file
    write_to_file(filename, debug, full_path)

class UART():
    def __init__(self):
        self.ser = serial.Serial("/dev/serial0",9600)
        self.ser.flush()
        pass

    def writeCoordinates(self, x, y, extrude=False):
        x = int(x)*10
        y = int(y)*10
        if((0 > x or x > 0xFFFF) or (0 > y or y > 0xFFFF)):
            print("Error")
            return -1

        num = x + (y << 16)

        cmd:int = 0b01 if extrude else 0b00

        cmd = (num << 8) + cmd

        cmd = int.to_bytes(cmd,5,"little")

        self.ser.write(cmd);
        return cmd

    def writeWait(self, t):
        t = int(t)
        cmd = (int.to_bytes(2+(t<<8),3,"little"))
        self.ser.write(t)

    def writeDone(self):
        cmd = int.to_bytes(3,1,"little")
        self.ser.write(cmd)

    def sendData(self, file):
        with open(file,"r") as f:
            file = f.read().strip()
            file += "\ndone"
            lines = file.split("\n")
            for i in range(len(lines)):
                line = lines[i]
                line = line.strip()
                line = line.split()
                print(i)
                print(line)
                if(line[0].strip() == "done"):
                    # continue
                    self.writeDone()
                if(line[0].strip() == "move_dry"):
                    # continue
                    self.writeCoordinates(line[1].strip(),line[2].strip(),True)
                if(line[0].strip() == "move_wet"):
                    # continue
                    self.writeCoordinates(line[1].strip(),line[2].strip(),True)
                if(line[0].strip() == "wait"):
                    continue
                    self.writeWait(line[1].strip())

#                time.sleep(.05)
#                continue

                data = self.ser.read()
                print(int.from_bytes(data,"little"))
                if(data == b'\x01'):
                    pass
                elif(data == b'\x00'):
                    pass
                elif(data == b'\x69'):
                    #resend
                    i -= 1
                else:
                    print("BUFFER FULL")
                    print(data)
                    i -= 1
                    while(self.ser.read() != b'\0x00'):
                        pass
                    print("SENDING MORE")
                time.sleep(.05)

    def close(self):
        self.ser.close()

if(__name__ == "__main__"):
    name = sys.argv[1]
    img = cv2.imread(name)
    edges = processImage(img, False)
    find_coordinates(edges,sys.argv[2])
    x = UART()
    x.sendData(sys.argv[2])
    x.close()
