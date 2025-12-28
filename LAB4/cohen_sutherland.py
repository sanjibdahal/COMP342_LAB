from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

x_min, y_min = 50, 50
x_max, y_max = 400, 300

INSIDE = 0
LEFT = 1
RIGHT = 2
BOTTOM = 4
TOP = 8

lines = [(20, 30, 450, 350), (100, 100, 300, 250), (200, 50, 500, 100)]

def compute_code(x, y):
    code = INSIDE
    if x < x_min:
        code |= LEFT
    elif x > x_max:
        code |= RIGHT
    if y < y_min:
        code |= BOTTOM
    elif y > y_max:
        code |= TOP
    return code

def cohen_sutherland_clip(x1, y1, x2, y2):
    code1 = compute_code(x1, y1)
    code2 = compute_code(x2, y2)
    accept = False
    while True:
        if code1 == 0 and code2 == 0:
            accept = True
            break
        elif (code1 & code2) != 0:
            break
        else:
            code_out = code1 if code1 != 0 else code2
            if code_out & TOP:
                x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1)
                y = y_max
            elif code_out & BOTTOM:
                x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1)
                y = y_min
            elif code_out & RIGHT:
                y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1)
                x = x_max
            elif code_out & LEFT:
                y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1)
                x = x_min
            if code_out == code1:
                x1, y1 = x, y
                code1 = compute_code(x1, y1)
            else:
                x2, y2 = x, y
                code2 = compute_code(x2, y2)
    return accept, x1, y1, x2, y2

def draw_dotted_line(x1, y1, x2, y2):
    glEnable(GL_LINE_STIPPLE)
    glLineStipple(1, 0x00FF)
    glBegin(GL_LINES)
    glVertex2f(x1, y1)
    glVertex2f(x2, y2)
    glEnd()
    glDisable(GL_LINE_STIPPLE)

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

    for x1, y1, x2, y2 in lines:
        accept, cx1, cy1, cx2, cy2 = cohen_sutherland_clip(x1, y1, x2, y2)
        glColor3f(1, 1, 1)
        if accept:
            if x1 != cx1 or y1 != cy1:
                draw_dotted_line(x1, y1, cx1, cy1)
            if x2 != cx2 or y2 != cy2:
                draw_dotted_line(x2, y2, cx2, cy2)
        else:
            draw_dotted_line(x1, y1, x2, y2)

        if accept:
            glColor3f(1, 0, 0)
            glLineWidth(2)
            glBegin(GL_LINES)
            glVertex2f(cx1, cy1)
            glVertex2f(cx2, cy2)
            glEnd()
    glFlush()

glutInit()
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
glutInitWindowSize(500, 400)
glutCreateWindow(b"Cohen-Sutherland Line Clipping")
glClearColor(0, 0, 0, 1)
glMatrixMode(GL_PROJECTION)
glLoadIdentity()
gluOrtho2D(0, 500, 0, 400)
glutDisplayFunc(display)
glutMainLoop()
