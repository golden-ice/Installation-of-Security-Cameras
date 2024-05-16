#!/usr/bin/env python3
import sys
import shlex
import re

from intersection import *

class Traffic(object):
    def __init__(self, coorAll={}):
        self.coorAll = coorAll
        self.lines = {}
        self.vertices = []
        self.V = {}
        self.E = []

    def add(self, road, coors):
        self.coorAll[road] = coors

    def rm(self, road):
        self.coorAll.pop(road)

    def mod(self, road, coors):
        self.coorAll.pop(road)
        self.coorAll[road] = coors

    def getCoor(self):
        return self.coorAll

    def getLines(self):
        return self.lines
    
    def getVertices(self):
        return self.vertices 

    # {(4, 2): 1, (4, 7): 2, (4, 8): 3, (1, 4): 4, (5, 8): 5}
    def getV(self):
        return self.V
    
    # [(1, 2), (2, 3), (4, 2), (2, 5)]
    def getE(self):
        return self.E
    
    def printV(self):
        print("V = {")
        for key in self.V:
            print("  " + str(self.V[key]) + ": ("  + str(round(key[0], 2)) + "," + str(round(key[1], 2)) + ")")
        print("}")
    
    def printE(self):
        print("E = {")
        for i in range(len(self.E)):
            if i != len(self.E) - 1:
                print("  <" + str(self.E[i][0]) +"," + str(self.E[i][1]) + ">,")
            else:
                print("  <" + str(self.E[i][0]) +"," + str(self.E[i][1]) + ">")
        print("}")
    
    # transfer the street coors to line segments of each street.
    '''
    road: Weber Street
    (2,-1)---(2,2)
    (2,2)---(5,5)
    (5,5)---(5,6)
    (5,6)---(3,8)
    road: King Street S
    (4,2)---(4,8)
    road: Davenport Road
    (1,4)---(5,8)
    '''
    def calLines(self):
        # reset self.lines
        self.lines = {}
        for key in self.coorAll:
            lineSegList = []
            for i in range(1, len(self.coorAll[key])):
                x1, y1 =self.coorAll[key][i-1][0], self.coorAll[key][i-1][1]
                x2, y2 =self.coorAll[key][i][0], self.coorAll[key][i][1]
                p1, p2 = Point(x1, y1), Point(x2, y2)
                lineSeg = LineSegment(p1, p2)
                lineSegList.append(lineSeg)
            self.lines[key] = lineSegList  

    # transfer the line segments to a vertex list
    # use after calLines()
    '''
    [[[(2, 2), (4, 4), (5, 5)], [(3, 8), (4, 7), (5, 6)]], [[(4, 2), (4, 4), (4, 7), (4, 8)]], [[(1, 4), (4, 7), (5, 8)]]]
    '''    
    def calIntersect(self):
        # reset self.vertices
        self.vertices = []
        roadList = []
        vertices = []
        for road in self.lines:
            roadList.append(self.lines[road])
        for i in range(len(roadList)):
            vertices.append([])
            for j in range(len(roadList[i])):
                vertices[i].append(set())
        for i in range(len(roadList)):
            for j in range(i+1, len(roadList)):
                for m in range(len(roadList[i])):
                    for n in range(len(roadList[j])):
                        intersec = intersection(roadList[i][m], roadList[j][n])
                        if intersec != None:
                            vertices[i][m].add(roadList[i][m].gets().get())
                            vertices[i][m].add(roadList[i][m].gete().get())
                            vertices[i][m].add(intersec.get())
                            vertices[j][n].add(roadList[j][n].gets().get())
                            vertices[j][n].add(roadList[j][n].gete().get())
                            vertices[j][n].add(intersec.get())
        
        verticesClean = []
        for i in range(len(vertices)):
            if vertices[i] == [set()]:
                continue
            verticesClean.append(vertices[i])   

        verticesCleanC = []   
        for i in range(len(verticesClean)):
            verticesCleanC.append([])
            for j in range(len(verticesClean[i])):
                if verticesClean[i][j] == set():
                    continue
                verticesCleanC[i].append(list(verticesClean[i][j]))
        
        for i in range(len(verticesCleanC)):
            for j in range(len(verticesCleanC[i])):
                verticesCleanC[i][j] = sorted(verticesCleanC[i][j], key=lambda x: (x[0], x[1]))
        
        self.vertices = verticesCleanC

    # cal E and V
    # use after calIntersect() 
    def calev(self):
        # reset V and E
        self.V = {}
        self.E = []
        if self.vertices != []:
            count = 1
            for i in range(len(self.vertices)):
                for j in range(len(self.vertices[i])):
                    for k in range(len(self.vertices[i][j])):
                        if self.V.get(self.vertices[i][j][k], 0) == 0:
                            self.V[self.vertices[i][j][k]] = count
                            count += 1
        
        for i in range(len(self.vertices)):
            for j in range(len(self.vertices[i])):
                for k in range(len(self.vertices[i][j]) - 1):
                    x = self.V.get(self.vertices[i][j][k], 0)
                    y = self.V.get(self.vertices[i][j][k+1], 0)
                    self.E.append((x, y))



def myParser(input):
    cmd, road = "", ""
    coors = []
    # coors = {}

    try:
        spInput = shlex.split(input)
        cmd = spInput[0]
    except Exception as e:
        print('Error: ' + str(e), file = sys.stderr)
        return cmd, road, coors

    if cmd == "gg":
        if len(spInput) > 1:
            raise Exception("too many arguments!")
    elif cmd == "rm":
        if len(spInput) < 2:
            raise Exception("need the street name for removing!")
        if len(spInput) > 2:
            raise Exception("too many arguments!")
        road = spInput[1]
    elif cmd == "add" or cmd == "mod":
        if len(spInput) < 3:
            raise Exception("need a street name and at least 2 coordinates!")
        road = spInput[1]
        coorInput = "".join([item for item in spInput[2:]])
        test = re.search(r"^(\([+-]?[0-9]+\,[+-]?[0-9]+\))+$", coorInput)
        if test:
            testA = test.string
            testB = testA.replace(")(", ") (")
            testC = testB.split()
            if len(testC) < 2:
                raise Exception("wrong format for coordinates!") 
        else:
            raise Exception("wrong format for coordinates!") 
        for i in range(0, len(testC)):
            if testC[i][0] != "(" or testC[i][-1] != ")":
                raise Exception("wrong format for coordinates!")    
            item = testC[i][1:-1].split(",")
            try:
                xcoor, ycoor = int(item[0]), int(item[1])
            except Exception as e:
                print('Error: ' + str(e), file = sys.stderr)
            coors.append((xcoor, ycoor))
            '''
            if coors == {}:
                coors[(xcoor, ycoor)] = 1
            if coors.get((xcoor, ycoor), 0) == 0:
                coors[(xcoor, ycoor)] = max(coors.values()) + 1
            '''
    else:
        raise Exception("wrong command!")
    return cmd, road, coors


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    traffic = Traffic({})
    
    # 1) read and preprocess the input
    while True:
        line = sys.stdin.readline()
        if not line.strip():
            sys.exit(0)
        # print("read a line:", line)
        try:
            cmd, road, coors = myParser(line)
            # print(cmd)
            # print(road)
            # print(coors)
            if cmd == "add":
                if traffic.getCoor().get(road, 0) != 0:
                    raise Exception("the road already exists!")
                traffic.add(road, coors)
            if cmd == "rm":
                try:
                    traffic.rm(road)
                except Exception as e:
                    print('Error: ' + str(e), file = sys.stderr)
            if cmd == "mod":
                try:
                    traffic.mod(road, coors)
                except Exception as e:
                    print('Error: ' + str(e), file = sys.stderr)
            # print(traffic.getCoor())
            if cmd == "gg":
                # print(traffic.getCoor())
                traffic.calLines()
                '''
                trafficLines = traffic.getLines()
                for road in trafficLines:
                    print("road: " + road)
                    for lineSegment in trafficLines[road]:
                        print(str(lineSegment))
                '''
                traffic.calIntersect()
                # print(traffic.getVertices())
                traffic.calev()
                # print(traffic.getV())
                # print(traffic.getE())
                traffic.printV()
                traffic.printE()
                



        except Exception as e:
            print('Error: ' + str(e), file = sys.stderr)
    
    # print("Finished reading input")
    # 2) calculte the intersections


    
    # return exit code 0 on successful termination
    sys.exit(0)

if __name__ == "__main__":
    main()
