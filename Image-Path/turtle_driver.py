import turtle
scale = 1
#
width = 100
height = 100

#Read in the paths from paths.txt
paths = []
with open('paths.txt') as f:
    # width = int(f.readline().strip()) * scale
    # height = int(f.readline().strip()) * scale
    for line in f:
        line = line.strip().split()
        if len(line) == 3:
            paths.append(line[0])
        # elif len(line) == 5:
        #     paths.append(line[2])
            coordinates = [line[-2], line[-1]]
            #Given coordinates are (0,0) at top left of image, positive y axis going downwards
            #Turtle coordinates are (0,0) in the center of the screen with normal +/- directions
            #Fractional coordinates don't cause errors
            x_coord = int(coordinates[0])*scale - (width/2)
            y_coord = -1*(int(coordinates[1])*scale - (height/2))
            paths.append([x_coord,y_coord])
        else:
            pass


sc = turtle.Screen()

# Set up the screen size and create turtle
sc.setup(width*2, height*2)
polly = turtle.Turtle()
polly.speed(5)

for pixel in paths:
    if pixel == "move_dry":
        polly.penup()
        # polly.pendown()
    elif pixel == "move_wet":
        polly.pendown()
    else:
        polly.goto(pixel[0], pixel[1])

    # polly.penup()
    # polly.goto(path[0][0], path[0][1])
    # polly.pendown()
    #
    # previous = path[0]
    # current = path[0]
    # for pixel in path:
    #     current = pixel
    #     if (current[0] - previous[0] > 1) or (current[1] - previous[1] > 1) or (current[0] - previous[0] < -1) or (current[1] - previous[1] < -1):
    #         polly.penup()
    #         polly.goto(pixel[0], pixel[1])
    #         polly.pendown()
    #     else:
    #         polly.goto(pixel[0], pixel[1])
    #     previous = pixel

turtle.done()