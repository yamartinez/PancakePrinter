#Toy Grouping Algorithm
#Preliminary concept for developing the paths to dispense batter in a logical order

#The first step of path identification is deciding how to group darkened pixels into continuous lines
#The next step is deciding which order to print each group, and the pixels within that group

#Contains two line groupings - a curve and a straight line
# 0 0 1 0 0
# 1 0 0 1 0
# 1 0 0 0 1
# 1 0 0 0 1
# 1 0 0 1 0
# 0 0 1 0 0

#Input coming in as an array of binary pixels
#Later, could refine grouping to include different colors
#This will be a numpy array
test_case_1 = [
    [0, 0, 1, 0, 0],
    [1, 0, 0, 1, 0],
    [1, 0, 0, 0, 1],
    [1, 0, 0, 0, 1],
    [1, 0, 0, 1, 0],
    [0, 0, 1, 0, 0]
]

test_case_2 = [
    [0, 0, 1, 0, 0],
    [1, 1, 0, 0, 1],
    [0, 0, 0, 0, 0],
    [1, 0, 0, 0, 1],
    [0, 1, 0, 1, 0],
    [0, 0, 1, 0, 0]
]


#List of 2D arrays of pixels to track each grouping grid, plus width/height of image in pixels
groupings = []
width = 5
height = 6
current_test = test_case_2

def check_surrounding_pixels(group, x, y):
    '''
    Checks the 8 pixels surrounding the (x,y) position in the provided group's grid, if they exist.
    Returns true if any of the surrounding pixels are 1.
    '''
    #North
    if (y - 1) >= 0:
        if group[y-1][x] == 1:
            return True
    #South
    if (y + 1) <= height - 1:
        if group[y+1][x] == 1:
            return True
    #East
    if (x + 1) <= width - 1:
        if group[y][x+1] == 1:
            return True
    #West
    if (x - 1) >= 0:
        if group[y][x-1] == 1:
            return True
    #Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if group[y-1][x+1] == 1:
            return True
    #Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if group[y-1][x-1] == 1:
            return True
    #Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if group[y+1][x+1] == 1:
            return True
    #Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if group[y+1][x-1] == 1:
            return True
    return False

def merge(group1, group2):
    '''
    Given two grids of identical size, creates a new grid containing all 1 values from both inputs.
    '''
    new_grid = [[0 for i in range(width)] for j in range(height)]
    for y in range(height):
        for x in range(width):
            if group1[y][x] == 1 or group2[y][x] == 1:
                new_grid[y][x] = 1
    return new_grid


# Method:
# Look at each pixel
# if darkened (1):
# 	decide if it is a group already
# 	Either make new group or add to an existing group
# 		In order to avoid searching through every element of a group,
# 		store each group as a 2D array so you can check elements next to it in constant time

for y in range(height):
    for x in range(width):
        if current_test[y][x] == 1:
            #Decide whether or not this pixel belongs to another group or not
            #To do so, check all 8 surrounding pixels in all existing groups
            #Groups may need to merge
                #ex. smile on a smiley face - the sides of the smile would initially be grouped separately going top-down on the image
            #In case a pixel is part of two or more groupings, merge the groupings

            #Determine which groups contain the current pixel
            group_indices = [] #Tracks the indices (in groupings) of the existing groups which should contain the current pixel
            for i in range(len(groupings)):
                if check_surrounding_pixels(groupings[i], x, y):
                    group_indices.append(i)

            #If no current groups contain the pixel, make a new group
            if len(group_indices) == 0:
                empty_grid = [[0 for i in range(width)] for j in range(height)]
                empty_grid[y][x] = 1
                groupings.append(empty_grid)
            else:
                #Add the pixel to the first group that contains it
                groupings[group_indices[0]][y][x] = 1

                #Merge all groups that contain the pixel
                while len(group_indices) > 1:
                    group2 = groupings[group_indices[-1]]
                    group1 = groupings[group_indices[-2]]
                    new_group = merge(group1, group2)

                    groupings[group_indices[-2]] = new_group
                    groupings.pop(group_indices[-1])
                    group_indices.pop()


print("Test Case:")
for x in current_test:
    print(x)

print()
print("Groupings")
for group in groupings:
    print()
    #print each grouping as a grid
    for row in group:
        print(row)