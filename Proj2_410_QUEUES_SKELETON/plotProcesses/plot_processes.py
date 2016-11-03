#!/usr/bin/env python3
"""Display the result of a run of the queues c++ program as a scatterplot

Opens a file one directory up reads in the comma seperated list of rows
Each row represents the ticcount and the process currently running

It then displays these points in a scatterplot where each processes
mark is the same color (ie all of process 8's points are blue)

Any points that are associated with process 0 are bogus, it really means
uninitialized
"""
import numpy as np
import matplotlib.pyplot as plt
import random

#super cheesy to hardcode this
FILE_WHERE_DATA_IS = '..\\results.txt'

def read_datafile(file_name):
    """loads the file data and converts it into a n,2 numpy array"""
    data = np.loadtxt(file_name, delimiter=',')
    return data

#pull points into an array
data = read_datafile(FILE_WHERE_DATA_IS)

#filter x and y out into 1 dimensional array
x= data[:,0]
y= data[:,1]

#set negative values=0 (a negative value means UNINITIALIZED)
x[x<0]=0
y[y<0]=0

#how many ticks?
numvals = x.size

plt.subplot(111)
plt.xlim(0, np.amax(x)+1)   #set range of plot
plt.ylim(0, np.amax(y)+1)
plt.grid(True)              #show a grid
plt.title("when a process is running")
plt.xlabel('tickcount')
plt.ylabel('process number')

#want the same color scheme every time
random.seed(0)

#want 1 unique color per unique process number
numb_unique_colors = int(np.amax(y))
colors = []
for i in range(numb_unique_colors+1):
    colors.append('#%06X' % random.randint(0, 0xFFFFFF))

#build an array of colors, 1 for each plot point
#points with the same process number will have the same color
all_y_colors=[colors[int(val)] for val in y]

#create and show
plt.scatter(x,y,s=70, c=all_y_colors)
plt.show()