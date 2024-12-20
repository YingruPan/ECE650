import re
import sys



class Point:
    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)
    def __str__(self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'



class Segment:
    def __init__(self, start, end):
        self.start = start
        self.end = end
    def __eq__(self, other):
        if isinstance(other, Segment):
            return (self.start == other.start and self.end == other.end) or (self.start == other.end and self.end == other.start)
        return False
    def __hash__(self):
        return hash((self.start, self.end))
    def __str__(self):
        return str(self.start) + '-->' + str(self.end)
    


# a dictionary to store streets information
class Streets_info:
    def __init__(self, streetdic = None):
        if streetdic is None:
            streetdic = {}
        self.streetdic = streetdic

    def add(self, streetname, coordinates):
        # if the streetname already exists
        if streetname in self.streetdic:
            raise Exception("Street already exists")
        # if the street doesnot have coordinate
        if coordinates == []:
            raise Exception("Please enter coordinates of the street")
        # if the street has only one coordinate
        if len(coordinates) == 1:
            raise Exception("Cannot add a street with one coordinate")
        # if different streets have same coordinates
        for exist_coordinates in self.streetdic.values():
            if coordinates == exist_coordinates:
                raise Exception("Cannot add different streets with same coordinates")
        self.streetdic[streetname] = coordinates

    def mod(self, streetname, coordinates):
        # if the streetname does not exist
        if streetname not in self.streetdic:
            raise Exception("Street does not exist")
        # if the street doesnot have coordinate
        if coordinates == []:
            raise Exception("Please enter new coordinates of the street")
        # if the street has only one coordinate
        if len(coordinates) == 1:
            raise Exception("Cannot reset a street with one coordinate")
        # if different streets have same coordinates
        for exist_coordinates in self.streetdic.values():
            if coordinates == exist_coordinates:
                raise Exception("Cannot reset a street with same coordinates as other streets")
        self.streetdic[streetname] = coordinates

    def rm(self, streetname):
        # if the streetname does not exist
        if streetname not in self.streetdic:
            raise Exception("Street does not exist")
        del self.streetdic[streetname]

    def get_streets(self):
        return self.streetdic
    


class Map:
    def __init__(self, vertexlist = None, edgeset = None, intersectionset = None, no = None):
        if vertexlist is None:
            vertexlist = []
        if edgeset is None:
            edgeset = set()
        if intersectionset is None:
            intersectionset = set()
        if no is None:
            no = 0
        self.vertexlist = vertexlist
        self.edgeset = edgeset
        self.intersectionset = intersectionset
        self.no = no
        
    def add_vertex(self, point):
        for exist_vertex in self.vertexlist:
            if point_is_equal_to(exist_vertex, point):
                return
        self.vertexlist.append(point)

    def get_vertices(self, current_streets):
        self.vertexlist = []
        streets = current_streets.get_streets()
        for street1, coordinate1 in streets.items():
            for i in range(len(coordinate1) - 1):
                if_intersect = False
                p1 = coordinate1[i]
                p2 = coordinate1[i + 1]
                l1 = Segment(p1, p2)
                for street2, coordinate2 in streets.items():
                    if street1 != street2:
                        for j in range(len(coordinate2) - 1):
                            p3 = coordinate2[j]
                            p4 = coordinate2[j + 1]
                            l2 = Segment(p3, p4)
                            # Determine the intersection
                            intersection = intersect(l1, l2)
                            if intersection:
                                # if overlaped, intersect function returns more than one point
                                if isinstance(intersection, tuple):
                                    self.add_vertex(intersection[0])
                                    self.add_vertex(intersection[1])
                                    self.intersectionset.add(intersection[0])
                                    self.intersectionset.add(intersection[1])
                                # if intersect or overlap at one point
                                else:
                                    self.add_vertex(intersection)
                                    self.intersectionset.add(intersection)
                                if_intersect = True
                if if_intersect:
                    self.add_vertex(p1)
                    self.add_vertex(p2)

    def get_edges(self, current_streets):
        self.edgeset = set()
        streets = current_streets.get_streets()
        for streetname, street_coordinates in streets.items():
            for i in range(len(street_coordinates) - 1):
                start = street_coordinates[i]
                end = street_coordinates[i + 1]
                vertices_on_segment = [start]
                intersections_on_segment = []
                for vertex in self.vertexlist:
                    if vertex in self.intersectionset and is_point_on_segment(vertex, start, end):
                        intersections_on_segment.append(vertex)
                intersections_on_segment.sort(key=lambda x: distance(start, x))
                vertices_on_segment.extend(intersections_on_segment)
                vertices_on_segment.append(end)
                for j in range(len(vertices_on_segment) - 1):
                    vertex1 = vertices_on_segment[j]
                    vertex2 = vertices_on_segment[j + 1]
                    if (self.is_intersection(vertex1) or self.is_intersection(vertex2)) and (vertex2 is not None and vertex1 is not None):
                        self.addedge(vertex1, vertex2)

    def addedge(self, vertex1, vertex2):
        if not point_is_equal_to(vertex1, vertex2):
            edge = Segment(vertex1, vertex2)
            self.edgeset.add(edge)

    def is_intersection(self, vertex):
        return vertex in self.intersectionset

    def gg_output(self):
        edgelist = list(self.edgeset)
        #for edge in edgelist:
        #    print("("+str(edge.start.x)+","+str(edge.start.y)+")-->("+str(edge.end.x)+","+str(edge.end.y)+")")
        #print('V = {')
        #for vertex in self.vertexlist:
        #    print(str(self.vertexlist.index(vertex) + 1) + ': (' + '{:.2f}'.format(vertex.x) + ',' + '{:.2f}'.format(vertex.y) + ')')
        #print('}')
        #with open('./a1output.txt', 'w') as file:
        #    file.write("V " + str(len(self.vertexlist)) + "\n")
        print("V " + str(len(self.vertexlist)))
        #    file.write("E {")
        print('E {', end = "")
        for edge in edgelist:
            if edgelist.index(edge) != len(edgelist) - 1:
                print('<' + str(self.vertexlist.index(edge.start) + 1) + ',' + str(self.vertexlist.index(edge.end) + 1) + '>,', end = "")
            else:
                print('<' + str(self.vertexlist.index(edge.start) + 1) + ',' + str(self.vertexlist.index(edge.end) + 1) + '>', end = "")
        print('}')
        sys.stdout.flush()
    


def point_is_equal_to(p1, p2):
    precise = 1e-6
    return abs(p1.x - p2.x) < precise and abs(p1.y - p2.y) < precise



def intersect(l1, l2):
    x1, y1 = l1.start.x, l1.start.y
    x2, y2 = l1.end.x, l1.end.y
    x3, y3 = l2.start.x, l2.start.y
    x4, y4 = l2.end.x, l2.end.y
    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    # if overlap or parallel
    if abs(xden) < 1e-10:
        # if on one line
        if abs((x1-x3)*(y2-y3) - (y1-y3)*(x2-x3)) < 1e-10:
            overlap_start = max(min(x1, x2), min(x3, x4)), max(min(y1, y2), min(y3, y4))
            overlap_end = min(max(x1, x2), max(x3, x4)), min(max(y1, y2), max(y3, y4))
            # if overlap at one point
            if overlap_start == overlap_end:
                return Point(*overlap_start)
            # if overlap at two points
            elif overlap_start[0] <= overlap_end[0] and overlap_start[1] <= overlap_end[1]:
                return Point(*overlap_start), Point(*overlap_end)
            # if not overlap
            else:
                return None  # Not overlap
        # if parallel and not on one line
        else:
            return None
    xcoor = xnum / xden
    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    ycoor = ynum / yden
    # if the intersect
    if (min(x1, x2) <= xcoor <= max(x1, x2) and min(y1, y2) <= ycoor <= max(y1, y2) and min(x3, x4) <= xcoor <= max(x3, x4) and min(y3, y4) <= ycoor <= max(y3, y4)):
        return Point(xcoor, ycoor)
    # if not intersect
    return None



# check if the point is on the segment
def is_point_on_segment(vertex, start, end):
    vertex_x, vertex_y = vertex.x, vertex.y
    start_x, start_y = start.x, start.y
    end_x, end_y = end.x, end.y
    precise = 1e-6
    # check if vertex between start and end
    if not (min(start_x, end_x) - precise <= vertex_x <= max(start_x, end_x) + precise) or not (min(start_y, end_y) - precise <= vertex_y <= max(start_y, end_y) + precise):
        return False
    # if vertical
    if abs(start_x - end_x) < precise:
        return abs(vertex_x - start_x) < precise
    # if horizontal
    if abs(start_y - end_y) < precise:
        return abs(vertex_y - start_y) < precise
    # else
    distance = abs((end_y - start_y) * vertex_x - (end_x - start_x) * vertex_y + end_x * start_y - end_y * start_x) / ((end_y - start_y)**2 + (end_x - start_x)**2)**0.5
    return distance < precise



def distance(p1, p2):
    return ((p1.x - p2.x)**2 + (p1.y - p2.y)**2)**0.5



# check the format of input line
def checkline(line):
    # check if "-" in the line is followed by number with no white spaces
    if '-' in line:
        parts = line.split('-')
        # make sure each "-" is followed by a number
        for part in parts[1:]:
            if not part or not part[0].isdigit():
                return False
    # check the format of line
    line1 = line.replace(" ", "")
    pattern = r'^[A-Za-z]+$|^[A-Za-z]+"[^"]+"(\(\-?\d+,\-?\d+\))*$'
    match = bool(re.match(pattern, line1))
    return match



# process the input line
def parseline(line):
    # first check the input format
    if checkline(line) == False:
        raise Exception('Invalid Input')
    else:
        line = line.strip()
    if not line:
        raise Exception('Empty Input')
    parts = line.split('"')
    cmd = parts[0].strip() 
    # if there is only a command
    if len(parts) == 1: 
        return cmd, "", ""
    streetname = parts[1].upper()  
    coords = line[line.index(parts[1]) + len(parts[1]):]  
    coords = coords.replace(" ", "")
    return cmd, streetname, coords



# change the string of coordinates into list
def parsecoordinates(coords):
    coordinates_of_points = []
    matches = re.findall(r'\((\-?\d+),\s*(\-?\d+)\)', coords)
    for x, y in matches:
        coordinates_of_points.append(Point(float(x), float(y)))
    return coordinates_of_points



def main():
    streets = Streets_info()
    map = Map()
    while True:
        line = sys.stdin.readline()
        if line == "":
                break
        if line == "\n":
                continue
        try:
            cmd, *args = parseline(line)
            if cmd == "add":
                streets.add(args[0], parsecoordinates(args[1]))
                #streets.get_streets()
            elif cmd == "mod":
                streets.mod(args[0], parsecoordinates(args[1]))
            elif cmd == "rm":
                streets.rm(args[0])
            elif cmd == "gg":
                map.get_vertices(streets)
                map.get_edges(streets)
                map.gg_output()
            else:
                raise Exception("Invalid command")
        except Exception as e:
            print('Error: ' + str(e), file = sys.stderr)
    sys.exit(0)



if __name__ == "__main__":
    main()
