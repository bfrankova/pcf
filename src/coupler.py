#!/usr/bin/python3

import re
import math
import sys

# represents single line segment
class Segment:
	def __init__(self, leftBorder, rightBorder, skew):
		self.leftBorder = leftBorder
		self.rightBorder = rightBorder
		self.skew = skew
		
	def __str__(self):
		return self.toString()
		
	def __repr__(self):
		return self.toString()

	def toString(self):
		return str(self.leftBorder) + " " + str(self.rightBorder) + " skew = " + str(self.skew)

# retrieve all segments from file
def getSegments(filename):
	lines = open(filename, 'r').readlines()
	segments = []

	for line in lines:
		matches = re.search(r"f[0-9]+\(x\) = \(([0-9\.]*)[^0-9]*([0-9\.]*)[^0-9]*([0-9\.]*)", line)
		try:
			segment = Segment(float(matches.groups()[0]), float(matches.groups()[1]), float(matches.groups()[2]))
			segments.append(segment)
		except:
			# print("not found")
			pass
	return segments

if(len(sys.argv) != 3):
	print("Not enough inputs.")
	exit(1)

#
newSegments1 = getSegments(sys.argv[1])
newSegments2 = getSegments(sys.argv[2])

print("Source segments 1:")
print(newSegments1)
print()
print("Source segments 2:")
print(newSegments2)
print()

# get result time range
minBorder = float("inf")
maxBorder = 0
#
for segment in newSegments1 + newSegments2:
	minBorder = min(minBorder, float(segment.leftBorder))
	maxBorder = max(maxBorder, float(segment.rightBorder))

if(maxBorder == 0 or minBorder == float("inf")):
	print("Average skew difference: *")
	exit(1)

# discretization
newPoints = []
#
for x in range(int(minBorder), int(maxBorder)):
	skew1 = float("nan")
	skew2 = float("nan")
	for segment1 in newSegments1:
		if(x > segment1.leftBorder and x < segment1.rightBorder):
			skew1 = segment1.skew
	for segment2 in newSegments2:
		if(x > segment2.leftBorder and x < segment2.rightBorder):
			skew2 = segment2.skew

	newPoints.append([x, abs(skew1-skew2)*1000])
# insert point to finish the last segment
newPoints.append([x+1, float("nan")])

# generate result segments
currentSegment = None
resultSegments = []
for point in newPoints:
	if not math.isnan(point[1]):
		if(currentSegment == None):
			currentSegment = Segment(point[0], 0, point[1])
	else:
		if(currentSegment != None):
			currentSegment.rightBorder = point[0]-1
			resultSegments.append(currentSegment)
			currentSegment = None
			
print("Target segments:")
print(resultSegments)
totalWidth = 0
for segment in resultSegments:
	totalWidth += segment.rightBorder - segment.leftBorder

# compute average skew
averageSkewDifference = 0
#
for segment in resultSegments:
	weight = (segment.rightBorder - segment.leftBorder)/totalWidth
	averageSkewDifference += weight * segment.skew

print()
if(averageSkewDifference == 0.0):
	print("Average skew difference: *")
else:
	print("Average skew difference:", ("%.2f" % averageSkewDifference))
