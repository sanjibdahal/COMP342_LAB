from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import sys

WINDOW_WIDTH = 1200
WINDOW_HEIGHT = 600

def init():
    glClearColor(0.0, 0.0, 0.0, 1.0)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0) # Coordinate system
    glMatrixMode(GL_MODELVIEW)

def draw_rect(x, y, w, h):
    """Draws a filled rectangle from (x, y) with width w and height h."""
    glBegin(GL_QUADS)
    glVertex2f(x, y)
    glVertex2f(x + w, y)
    glVertex2f(x + w, y + h)
    glVertex2f(x, y + h)
    glEnd()

def draw_S(x, y, w, h, thickness):
    draw_rect(x, y + h - thickness, w, thickness)
    draw_rect(x, y + (h / 2) - (thickness / 2), w, thickness)
    draw_rect(x, y, w, thickness)
    draw_rect(x, y + (h / 2), thickness, h / 2)
    draw_rect(x + w - thickness, y, thickness, h / 2)

def draw_A(x, y, w, h, thickness):
    draw_rect(x, y, thickness, h)
    draw_rect(x + w - thickness, y, thickness, h)
    draw_rect(x, y + h - thickness, w, thickness)
    draw_rect(x, y + (h / 2), w, thickness)

def draw_N(x, y, w, h, thickness):
    draw_rect(x, y, thickness, h)
    draw_rect(x + w - thickness, y, thickness, h)
    
    glBegin(GL_QUADS)
    glVertex2f(x, y + h)
    glVertex2f(x + thickness, y + h)
    glVertex2f(x + w, y)
    glVertex2f(x + w - thickness, y)
    glEnd()

def draw_J(x, y, w, h, thickness):
    draw_rect(x, y + h - thickness, w, thickness)
    draw_rect(x + w/2, y + thickness, thickness, h - thickness)
    draw_rect(x + w/2, y + thickness, thickness, h - thickness)
    draw_rect(x, y, w/2 + thickness, thickness)
    draw_rect(x, y, thickness, h/3)

def draw_I(x, y, w, h, thickness):
    draw_rect(x, y + h - thickness, w, thickness)
    draw_rect(x, y, w, thickness)
    draw_rect(x, y, w, thickness)
    draw_rect(x + (w / 2) - (thickness / 2), y, thickness, h)

def draw_B(x, y, w, h, thickness):
    draw_rect(x, y, thickness, h)
    draw_rect(x, y + h - thickness, w * 0.8, thickness)
    draw_rect(x, y + (h / 2) - (thickness / 2), w * 0.8, thickness)
    draw_rect(x, y + (h / 2) - (thickness / 2), w * 0.8, thickness)
    draw_rect(x, y, w * 0.8, thickness)
    draw_rect(x + w * 0.8 - thickness, y + h/2, thickness, h/2)
    draw_rect(x + w * 0.8 - thickness, y, thickness, h/2)

def display():
    glClear(GL_COLOR_BUFFER_BIT)
    
    start_x = -0.9
    y = -0.2
    h = 0.5
    w = 0.25
    gap = 0.05
    thickness = 0.05
    
    # S
    glColor3f(1.0, 0.0, 0.0)
    draw_S(start_x, y, w, h, thickness)
    
    # A
    glColor3f(0.0, 1.0, 0.0)
    draw_A(start_x + (w + gap), y, w, h, thickness)
    
    # N
    glColor3f(0.0, 0.0, 1.0)
    draw_N(start_x + 2*(w + gap), y, w, h, thickness)
    
    # J
    glColor3f(1.0, 1.0, 0.0)
    draw_J(start_x + 3*(w + gap), y, w, h, thickness)
    
    # I
    glColor3f(0.0, 1.0, 1.0)
    draw_I(start_x + 4*(w + gap), y, w, h, thickness)
    
    # B
    glColor3f(1.0, 0.0, 1.0)
    draw_B(start_x + 5*(w + gap), y, w, h, thickness)
    
    glFlush()

def main():
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutInitWindowPosition(100, 100)
    glutCreateWindow(b"SANJIB - OpenGL")
    init()
    glutDisplayFunc(display)
    glutMainLoop()

if __name__ == "__main__":
    main()
