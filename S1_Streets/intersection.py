
class Point(object):
    def __init__ (self, x, y):
        self.x = x
        self.y = y
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'
    def getx(self):
        return self.x
    def gety(self):
        return self.y
    def get(self):
        return (self.x, self.y)

class LineSegment(object):
    def __init__ (self, start, end):
        self.start = start
        self.end = end

    def __str__(self):
        return str(self.start) + '---' + str(self.end)

    def gets(self):
        return self.start

    def gete(self):  
        return self.end

'''
Reference: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
'''
def intersection (l1, l2):
    x1, y1 = l1.gets().getx(), l1.gets().gety()
    x2, y2 = l1.gete().getx(), l1.gete().gety()
    x3, y3 = l2.gets().getx(), l2.gets().gety()
    x4, y4 = l2.gete().getx(), l2.gete().gety()
    # print(x1, y1)
    # print(x2, y2)
    # print(x3, y3)
    # print(x4, y4)

    denomi = (x1 - x2)*(y3 - y4)-(y1 - y2)*(x3 - x4)
    
    # The two line segments are overlapping or parallel
    if denomi == 0:
        return None
    
    t = ((x1 - x3)*(y3 -y4)-(y1 - y3)*(x3 - x4)) / denomi
    u = ((x1 - x3)*(y1 -y2)-(y1 - y3)*(x1 - x2)) / denomi
    # The two line segments are not intersected
    if t < 0 or t > 1 or u < 0 or u > 1:
        return None
    
    xcoor = x1 + t*(x2 - x1)
    ycoor = y1 + t*(y2 - y1)
    # print(xcoor, ycoor)
    if int(xcoor) == xcoor:
        xcoor = int(xcoor)
    if int(ycoor) == ycoor:
        ycoor = int(ycoor)
    return Point(xcoor, ycoor)

