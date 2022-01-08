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

print("Started")

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

def findCoordinates(image_name, filename, debug = False):
    #List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
    groupings = []
    height, width = edges.shape
    background_color = 0
    edge_color = 255

    # #Skeletonization
    binary_edges = np.zeros((height, width), np.uint8)
    if debug == True:
        print("height:", height)
        print("width:", width)
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

    current_test = skeleton

    def find_surrounding_pixels(group, x,y):
        '''
        Given an (x,y) coordinate, checks the surrounding pixels in the current_test
        Erases the pixels from current_test and returns a list of colored pixels
        '''
        # global current_test
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

    def wet_or_dry(pixel1, pixel2):
        #Determines whether to move_wet or move_dry
        #Only move_wet if the pixels are right next to each other
        if (abs(pixel1[0] - pixel2[0]) <= 1) and (abs(pixel1[1] - pixel2[1]) <= 1):
            return "move_wet"
        return "move_dry"

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

    #Removes unnecessary in-between pixels which follow the same slope
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
        #Need to consider whether this is moving wet or dry
        #First pixel of a group will always be move_dry
        #Every other pixel should be move_wet unless the grouping jumps back to an intersection

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

    #Add in waits
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

    if debug == True:
        total = 0
        for path in draw_paths_optimized:
            for pixel in path:
                total += 1
                print(pixel)
        print("Total lines:",total)
    # 2587 pixels for the snowman image, un-optimized

    #Export this information to a file
    with open(filename, 'w') as f:
        for path in draw_paths_optimized:
            for i in range(len(path)):
                pixel = path[i]
                if pixel == "move_dry" or pixel == "move_wet":
                    f.write(pixel + " ")
                elif pixel == "wait 1":
                    pass#f.write(pixel + "\n")
                else:
                    f.write(str(pixel[0]) + " " + str(pixel[1]) + "\n")
        f.write("done\n")

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
            print(file)
            file = f.read()
            lines = file.split("\n")
            self.ser.flush()
            for i in range(len(lines)-1):
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
                    self.writeCoordinates(line[1].strip(),line[2].strip())
                if(line[0].strip() == "move_wet"):
                    # continue
                    self.writeCoordinates(line[1].strip(),line[2].strip(),True)
                if(line[0].strip() == "wait"):
                    # continue
                    self.writeWait(line[1].strip())
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
    try:
        with open(sys.argv[2],'r') as f:
            pass
        print("path already exists")
    except:
        print("Starting now")
        name = sys.argv[1]
        img = cv2.imread(name)
        edges = processImage(img, False)
        print("Edges done")
        findCoordinates(edges,sys.argv[2])
        print("Coordinates done")
    finally:
        x = UART()
        x.sendData(sys.argv[2])
        x.close()
