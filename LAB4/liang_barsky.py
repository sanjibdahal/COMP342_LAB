from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *

x_min, y_min = 50, 50
x_max, y_max = 400, 300

lines = [(20, 30, 450, 350), (100, 100, 300, 250), (0, 300, 350, 0)]

def liang_barsky_clip(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1

    p = [-dx, dx, -dy, dy]
    q = [x1 - x_min, x_max - x1, y1 - y_min, y_max - y1]

    u1, u2 = 0, 1

    for i in range(4):
        if p[i] == 0:
            if q[i] < 0:
                return None
        else:
            u = q[i] / p[i]
            if p[i] < 0:
                u1 = max(u1, u)
            else:
                u2 = min(u2, u)
    if u1 > u2:
        return None

    cx1 = x1 + u1 * dx
    cy1 = y1 + u1 * dy
    cx2 = x1 + u2 * dx
    cy2 = y1 + u2 * dy

    return (cx1, cy1, cx2, cy2, u1, u2)

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
        result = liang_barsky_clip(x1, y1, x2, y2)

        glColor3f(1, 1, 1)
        if result:
            cx1, cy1, cx2, cy2, u1, u2 = result
            if u1 > 0:
                draw_dotted_line(x1, y1, cx1, cy1)
            if u2 < 1:
                draw_dotted_line(cx2, cy2, x2, y2)
        else:
            draw_dotted_line(x1, y1, x2, y2)

        if result:
            glColor3f(0, 0.6, 1)
            glLineWidth(2)
            glBegin(GL_LINES)
            glVertex2f(cx1, cy1)
            glVertex2f(cx2, cy2)
            glEnd()
    glFlush()

glutInit()
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
glutInitWindowSize(600, 450)
glutCreateWindow(b"Liang-Barsky Line Clipping")
glClearColor(0, 0, 0, 1)
glMatrixMode(GL_PROJECTION)
glLoadIdentity()
gluOrtho2D(0, 600, 0, 450)
glutDisplayFunc(display)
glutMainLoop()
