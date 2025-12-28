from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

x_min, y_min = 100, 100
x_max, y_max = 400, 300

polygon = [(50, 50), (200, 350), (450, 250), (350, 50)]

def clip_polygon(polygon):
    def clip_edge(polygon, edge):
        new_poly = []
        for i in range(len(polygon)):
            curr = polygon[i]
            prev = polygon[i - 1]
            
            if inside(curr, edge):
                if inside(prev, edge):
                    new_poly.append(curr)
                else:
                    new_poly.append(intersection(prev, curr, edge))
                    new_poly.append(curr)
            else:
                if inside(prev, edge):
                    new_poly.append(intersection(prev, curr, edge))
        return new_poly

    def inside(p, edge):
        x, y = p
        if edge == 'left':
            return x >= x_min
        elif edge == 'right':
            return x <= x_max
        elif edge == 'bottom':
            return y >= y_min
        elif edge == 'top':
            return y <= y_max

    def intersection(p1, p2, edge):
        x1, y1 = p1
        x2, y2 = p2
        if edge in ['left', 'right']:
            x_edge = x_min if edge == 'left' else x_max
            y = y1 + (y2 - y1) * (x_edge - x1) / (x2 - x1)
            return (x_edge, y)
        else:
            y_edge = y_min if edge == 'bottom' else y_max
            x = x1 + (x2 - x1) * (y_edge - y1) / (y2 - y1)
            return (x, y_edge)

    edges = ['left', 'right', 'bottom', 'top']
    clipped_poly = polygon[:]
    for edge in edges:
        clipped_poly = clip_edge(clipped_poly, edge)
    return clipped_poly

def draw_polygon_outline(vertices, color, line_width=2):
    glColor3f(*color)
    glLineWidth(line_width)
    glBegin(GL_LINE_LOOP)
    for x, y in vertices:
        glVertex2f(x, y)
    glEnd()

def draw_filled_polygon(vertices, color):
    glColor3f(*color)
    glBegin(GL_POLYGON)
    for x, y in vertices:
        glVertex2f(x, y)
    glEnd()

def display():
    glClear(GL_COLOR_BUFFER_BIT)
    
    glColor3f(0, 1, 0)
    glLineWidth(3)
    glBegin(GL_LINE_LOOP)
    glVertex2f(x_min, y_min)
    glVertex2f(x_max, y_min)
    glVertex2f(x_max, y_max)
    glVertex2f(x_min, y_max)
    glEnd()
    
    draw_polygon_outline(polygon, (1, 1, 1), 2)
    
    clipped_poly = clip_polygon(polygon)
    draw_filled_polygon(clipped_poly, (0, 0.6, 1))

    glFlush()

glutInit()
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
glutInitWindowSize(600, 450)
glutCreateWindow(b"Sutherland-Hodgeman Filled Polygon Clipping")
glClearColor(0, 0, 0, 1)
glMatrixMode(GL_PROJECTION)
glLoadIdentity()
gluOrtho2D(0, 600, 0, 450)
glutDisplayFunc(display)
glutMainLoop()
