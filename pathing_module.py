import sys
from threading import current_thread
import cv2
import numpy as np
import math
from skimage.morphology import skeletonize
from numpy import zeros, array, where

# Constants that can be changed by users
background_color = 0  # Black
edge_color = 255  # White

def make_skeleton(image_name, debug=False):
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
            all_pixels.append([x, y - 1])

    # South
    if (y + 1) <= height - 1:
        if group[y + 1, x] == edge_color:
            colored_pixels.append([y + 1, x])
            current_test[y + 1, x] = color_num
            all_pixels.append([x, y + 1])
    # East
    if (x + 1) <= width - 1:
        if group[y, x + 1] == edge_color:
            colored_pixels.append([y, x + 1])
            current_test[y, x + 1] = color_num
            all_pixels.append([x + 1, y])
    # West
    if (x - 1) >= 0:
        if group[y, x - 1] == edge_color:
            colored_pixels.append([y, x - 1])
            current_test[y, x - 1] = color_num
            all_pixels.append([x - 1, y])
    # Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y - 1, x + 1] == edge_color:
            colored_pixels.append([y - 1, x + 1])
            current_test[y - 1, x + 1] = color_num
            all_pixels.append([x + 1, y - 1])
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
            colored_pixels.append([x, y - 1])
            grid[y - 1, x] = background_color
    # South
    if (y + 1) <= height - 1:
        if grid[y + 1, x] > 0:
            colored_pixels.append([x, y + 1])
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
        if grid[y - 1, x + 1] > 0:
            colored_pixels.append([x + 1, y - 1])
            grid[y - 1, x + 1] = background_color
    # Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if grid[y - 1, x - 1] > 0:
            colored_pixels.append([x - 1, y - 1])
            grid[y - 1, x - 1] = background_color
    # Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if grid[y + 1, x + 1] > 0:
            colored_pixels.append([x + 1, y + 1])
            grid[y + 1, x + 1] = background_color
    # Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if grid[y + 1, x - 1] > 0:
            colored_pixels.append([x - 1, y + 1])
            grid[y + 1, x - 1] = background_color
    return colored_pixels, grid


def find_groups_and_centers():
    # Dry Edge = 255, Group 0 = 254, Group 1 = 253, ... Group N = 255 - (N+1)
    # For now, assume less than 2
    # in the future, account for this - check to make sure you can go above 255 first
    x_centers = []
    y_centers = []
    pixels_per_group = []
    a_pixel_per_group = []  # [x,y] format
    all_pixels_per_group = []  # Used as a tool later
    group_num = 0  # Initialize group numbers at 0
    for y in range(height):
        for x in range(width):
            if current_test[y, x] == edge_color:
                # If edge color is found, make new grouping
                # empty_grid = np.zeros((height, width), np.uint8)
                # empty_grid[y, x] = edge_color

                # If a pixel is found, set up to track info
                x_sum = 0
                y_sum = 0
                num_pixels = 0
                a_pixel_per_group.append([x, y])
                all_pixels = [[x, y]]

                # Convert found pixel to match the group's color
                color_num = 255 - (group_num + 1)
                current_test[y, x] = color_num

                # Find and convert the rest of the pixels to the color_num
                to_add, all_pixels = find_surrounding_pixels(current_test, x, y, color_num, all_pixels)
                while len(to_add) != 0:
                    pixel_to_add = to_add[0]
                    # empty_grid[pixel_to_add[0], pixel_to_add[1]] = edge_color
                    x_sum += pixel_to_add[1]
                    y_sum += pixel_to_add[0]
                    num_pixels += 1
                    ret_to_add, all_pixels = find_surrounding_pixels(current_test, pixel_to_add[1], pixel_to_add[0],
                                                                     color_num, all_pixels)
                    to_add += ret_to_add
                    to_add.pop(0)

                # groupings.append(empty_grid)
                x_centers.append(x_sum)
                y_centers.append(y_sum)
                pixels_per_group.append(num_pixels)
                all_pixels_per_group.append(all_pixels)
                group_num += 1  # Set this up to be ready for the next group
                # Also, at any point in time, group_num = the number of completed groups
    return x_centers, y_centers, pixels_per_group, a_pixel_per_group, all_pixels_per_group


def find_distance(pixel1, pixel2):
    x1 = pixel1[0]
    y1 = pixel1[1]
    x2 = pixel2[0]
    y2 = pixel2[1]
    return math.sqrt((y2 - y1) ** 2 + (x2 - x1) ** 2)


def find_top_left():
    '''
    Searches the global current_test grid for the top-left colored pixel
    '''
    global current_test, width, height

    #Search top left triangle
    for edge in range(width):
        for diagonal in range(min(edge + 1, height)):
            x = edge - diagonal
            y = diagonal
            if current_test[y, x] != 0:
                return [x, y]
    # Search bottom right triangle
    for edge in range(height - 1, -1, -1):
        for diagonal in range(min(edge + 1, width)):
            x = width - 1 - diagonal
            y = (height - 1 - edge) + diagonal
            if current_test[y, x] != 0:
                return [x, y]
    return [0,0]


def optimize_paths_with_movedry2(draw_paths):
    '''
    Keeps every third pixel as well as important jumping point pixels
    '''
    keep = "one"
    optimized_path = []

    for i in range(1, len(draw_paths)-2, 2):
        pixel = draw_paths[i]
        pixel_instruction = draw_paths[i - 1]
        next_pixel = draw_paths[i+2]
        next_pixel_instruction = draw_paths[i - 1 + 2]

        if pixel_instruction == "move_dry" or next_pixel_instruction == "move_dry":
            optimized_path.append(pixel_instruction)
            optimized_path.append(pixel)
            keep = "one"
        else:
            if keep == "one":
                optimized_path.append(pixel_instruction)
                optimized_path.append(pixel)

        #Next indicator
        if keep =="one":
            keep = "two"
        elif keep == "two":
            keep = "three"
        elif keep == "three":
            keep = "one"

    #add final pixel and intruction
    optimized_path.append(draw_paths[-2])
    optimized_path.append(draw_paths[-1])
    return optimized_path


def optimize_paths_without_movedry2(draw_paths):
    '''
    Keeps every third pixel as well as important jumping point pixels
    '''
    keep = "one"
    optimized_path = []

    for i in range(1, len(draw_paths)-2, 2):
        pixel = draw_paths[i]
        pixel_instruction = draw_paths[i - 1]
        next_pixel = draw_paths[i+2]
        next_pixel_instruction = draw_paths[i - 1 + 2]

        if pixel_instruction != next_pixel_instruction:
            optimized_path.append(pixel_instruction)
            optimized_path.append(pixel)
            keep = "three" #Sets it up so both this pixel and the next pixel will be covered
        else:
            if keep == "one":
                optimized_path.append(pixel_instruction)
                optimized_path.append(pixel)

        #Next indicator
        if keep =="one":
            keep = "two"
        elif keep == "two":
            keep = "three"
        elif keep == "three":
            keep = "one"

    #Add final pixel and intruction
    optimized_path.append(draw_paths[-2])
    optimized_path.append(draw_paths[-1])
    return optimized_path


def add_waits(path):
    path.append("end")
    prev_instruction = "move_dry"
    i = 0
    while path[i] != "end":
        pixel = path[i]
        if pixel == "move_dry" or pixel == "move_wet":
            if pixel == "move_dry" and prev_instruction == "move_wet":
                path.insert(i, "wait 5000")
                i += 2
            else:
                i += 1
            prev_instruction = pixel
        else:
            i += 1
    path.remove("end")
    return path


def debug_print(draw_paths_optimized):
    total = 0
    for pixel in draw_paths_optimized:
        if pixel == "move_dry" or pixel == "move_wet" or pixel == "move_dry2":
            x=5
            # print(pixel, end=" ")
        elif pixel == "wait 5000":
            x=5
            # print(pixel)
        else:
            # print(pixel[0], pixel[1])
            total += 1
    print("Total instructions:", total)


def write_to_file(filename, debug, full_path):
    with open(filename, 'w') as f:
        if debug:
            f.write(str(width) + "\n")
            f.write(str(height) + "\n")
        for pixel in full_path:
            if pixel == "move_dry" or pixel == "move_wet" or pixel == "move_dry2":
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

    # Check outer edges of the square made by the bounds
    # Top and bottom edges of square
    for x in range(lower_x, upper_x + 1):
        # Check lower y
        y = lower_y
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x, y]
        # Check upper y
        y = upper_y
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x, y]

    # Left and right edges of the square
    for y in range(lower_y + 1, upper_y):
        # Check lower x
        x = lower_x
        if (0 <= x < width) and (0 <= y < height):
            if grid[y, x] > 0:
                return [x, y]
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
    # print(current_test[0,0])
    # print("target", target_color)
    # for y in range(height):
    #     for x in range(width):
    #         print(current_test[y][x], end = "")
    #     print()
    # Populate with target-colored pixels unvisited in updated
    for x in range(width):
        for y in range(height):
            # print("wht")
            if grid[y, x] == target_color:
                # print("WOOP")
                target_group[y, x] = target_color
                num += 1
    # print(num, target_color)
    return target_group


def execute_best_path(start_pixel, end_pixel, grid):
    '''
    Traverses from the starting pixel to the ending pixel within colored pixels on the grid
    Returns a list of pixels to the end_pixel, not including the start_pixel
    '''
    if start_pixel == end_pixel:
        return []

    #Use BFS to find the shortest path to the end pixel
    #Structure inspiration from https://stackoverflow.com/questions/8922060/how-to-trace-the-path-in-a-breadth-first-search
    #Maintain a list of lists to keep track of paths
    current = start_pixel
    grid[current[1], current[0]] = 0 #Keep track of visited pixels by setting to 0 in grid
    paths_queue = [[current]]
    while paths_queue:
        #Get the first path from the queue
        path = paths_queue.pop(0)
        #Get the last pixel from from path
        current = path[-1]
        #If the path is found:
        if current == end_pixel:
            return path
        #Find adjacent nodes, construct a new path, and push to the queue
        adjacents, grid = find_surrounding_pixel_list(grid, current[0], current[1])
        for adj in adjacents:
            new_path = list(path)
            new_path.append(adj)
            paths_queue.append(new_path)
    return []


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
        # Go in one direction until you can't
        while next != 0:
            # print("hi2")
            current = next
            current_color = current_test[current[1], current[0]]
            full_path.append("move_wet")
            full_path.append(current)
            updated[current[1], current[0]] = 0
            next = check_circle(current, 1, updated)
        # Reached a dead end - sonar search to find a "target point"
        # If you cannot find a target point, traversal is finished
        radius = 1
        target_point = 0
        while radius < max(width, height):
            # print("hi3")
            target_point = check_circle(current, radius, updated)
            if target_point != 0:
                break
            radius += 1
        if target_point == 0:
            #The image has been fully drawn
            #Traverse back to the starting pixel and then return the full path
            target_point = start

            # Construct a current_group to allow for updating in dfs
            current_group = make_target_group(current_color, current_test)

            # Find the closest pixel (in the current group) to the target_point
            closest_pixel = 0
            # Sonar outward from target_point until a member of the current_group is found
            radius = 1
            while closest_pixel == 0:
                closest_pixel = check_circle(target_point, radius, current_group)
                radius += 1

            # Execute best path to the closest_pixel from current (they are all in the same group)
            new_path = execute_best_path(current, closest_pixel, current_group)

            # Add the new_path to the full_path
            for pixel in new_path:
                if updated[pixel[1], pixel[0]] == 0:  # already visited
                    full_path.append("move_dry") #move_dry2
                else:
                    full_path.append("move_wet")
                full_path.append(pixel)
                updated[pixel[1], pixel[0]] = 0

            # Dry jump to the target point if it wasn't already found
            if target_point != full_path[-1]:
                full_path.append("move_dry")
                full_path.append(target_point)
                updated[target_point[1], target_point[0]] = 0

            return full_path

        # Construct a current_group to allow for updating in dfs
        current_group = make_target_group(current_color, current_test)

        # Find the closest pixel (in the current group) to the target_point
        closest_pixel = 0
        #Sonar outward from target_point until a member of the current_group is found
        radius = 1
        while closest_pixel == 0:
            closest_pixel = check_circle(target_point, radius, current_group)
            radius += 1

        #Execute best path to the closest_pixel from current (they are all in the same group)
        new_path = execute_best_path(current, closest_pixel, current_group)

        #Add the new_path to the full_path
        for pixel in new_path:
            if updated[pixel[1], pixel[0]] == 0:  # already visited
                full_path.append("move_dry") #move_dry2
            else:
                full_path.append("move_wet")
            full_path.append(pixel)
            updated[pixel[1], pixel[0]] = 0

        # Dry jump to the target point if it wasn't already found
        if target_point != full_path[-1]:
            full_path.append("move_dry")
            full_path.append(target_point)
            updated[target_point[1], target_point[0]] = 0

        # Set up for the next cycle
        current = target_point
        next = check_circle(current, 1, updated)


def find_coordinates(image, filename, debug=False):
    # Establish globals to share with other functions
    global current_test, background_color, edge_color, groupings, width, height

    # List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
    groupings = []
    height, width = image.shape

    # current_test = make_skeleton(image, debug)

    current_test = image

    # Find each group and track the cumulative placement of pixels in each group
    x_centers, y_centers, pixels_per_group, a_pixel_per_group, all_pixels_per_group = find_groups_and_centers()

    #Find the starting pixel (top-left colored pixel)
    true_start = find_top_left()

    # Pathing - make updated grid to track visited
    updated = np.zeros((height, width), np.uint8)
    for y in range(height):
        for x in range(width):
            if current_test[y, x] > 0:
                number = current_test[y, x]
                updated[y, x] = number
    full_path = find_balanced_path(updated, current_test, true_start)

    # Removes unnecessary in-between pixels which follow the same slope
    full_path = optimize_paths_without_movedry2(full_path)

    # Add in waits - optional based on embedded code
    # full_path = add_waits(full_path)

    if debug:
        debug_print(full_path)

    # Export this information to a file
    write_to_file(filename, debug, full_path)

