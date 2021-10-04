#Toy Grouping Algoirithm
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


#List of lists of pixels to track each group, plus width/height in pixels
groupings = []
width = 5
height = 6

def find_connecting_pixels(x, y):
    #North
    if (y - 1) >= 0:
        if test_case_1[y-1][x] == 1:
            #Add to group
            groupings[-1].append([x, y-1])
            #Change the pixel
            test_case_1[y-1][x] = 0
            #Call recursively
            find_connecting_pixels(x, y-1)
    #South
    if (y + 1) <= height - 1:
        if test_case_1[y+1][x] == 1:
            #Add to group
            groupings[-1].append([x, y+1])
            #Change the pixel
            test_case_1[y+1][x] = 0
            #Call recursively
            find_connecting_pixels(x, y+1)
    #East
    if (x + 1) <= width - 1:
        if test_case_1[y][x+1] == 1:
            #Add to group
            groupings[-1].append([x+1, y])
            #Change the pixel
            test_case_1[y][x+1] = 0
            #Call recursively
            find_connecting_pixels(x+1, y)
    #West
    if (x - 1) >= 0:
        if test_case_1[y][x-1] == 1:
            #Add to group
            groupings[-1].append([x-1, y])
            #Change the pixel
            test_case_1[y][x-1] = 0
            #Call recursively
            find_connecting_pixels(x-1, y)
    #Northeast
    if (y - 1) >= 0 and (x + 1) <= width - 1:
        if test_case_1[y-1][x+1] == 1:
            #Add to group
            groupings[-1].append([x+1, y-1])
            #Change the pixel
            test_case_1[y-1][x+1] = 0
            #Call recursively
            find_connecting_pixels(x+1, y-1)
    #Northwest
    if (y - 1) >= 0 and (x - 1) >= 0:
        if test_case_1[y-1][x-1] == 1:
            #Add to group
            groupings[-1].append([x-1, y-1])
            #Change the pixel
            test_case_1[y-1][x-1] = 0
            #Call recursively
            find_connecting_pixels(x-1, y-1)
    #Southeast
    if (y + 1) <= height - 1 and (x + 1) <= width - 1:
        if test_case_1[y+1][x+1] == 1:
            #Add to group
            groupings[-1].append([x+1, y+1])
            #Change the pixel
            test_case_1[y+1][x+1] = 0
            #Call recursively
            find_connecting_pixels(x+1, y+1)
    #Southwest
    if (y + 1) <= height - 1 and (x - 1) >= 0:
        if test_case_1[y+1][x-1] == 1:
            #Add to group
            groupings[-1].append([x-1, y+1])
            #Change the pixel
            test_case_1[y+1][x-1] = 0
            #Call recursively
            find_connecting_pixels(x-1, y+1)


#Method: search every pixel. upon finding a darkened pixel, recursively track all the darkened pixels around it
#Future additions: how should we decide how to better order our pixels within our lines?
    #Choose a starting point with neighbors in only one direction?
    #What about loops of pixels?

for x in test_case_1:
    print(x)

for y in range(height):
    for x in range(width):
        if test_case_1[y][x] == 1:
            #Only initialize new grouping at the beginning
            groupings.append([])
            groupings[-1].append([x, y])
            # Change the pixel
            test_case_1[y][x] = 0
            find_connecting_pixels(x, y)
print()
for x in test_case_1:
    print(x)

print()
for entry in groupings:
    print(entry)