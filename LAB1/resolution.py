from OpenGL.GL import *
from OpenGL.GLUT import *

def print_resolution():
    width  = glutGet(GLUT_SCREEN_WIDTH)
    height = glutGet(GLUT_SCREEN_HEIGHT)
    print("Screen Resolution:", width, "x", height)


glutInit()
print_resolution()