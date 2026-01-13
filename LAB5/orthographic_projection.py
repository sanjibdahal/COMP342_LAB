from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import sys

# Vertices coordinates (x, y, z)
vertices = (
    (2, -2, -1), (2, -2, 1), (2, 2, 1), (2, 2, -1),
    (-2, -2, -1), (-2, -2, 1), (-2, 2, 1), (-2, 2, -1),
    (0, 3.5, 0) # Apex of the roof
)

# Edges (connecting vertex indices)
edges = (
    (0,1), (1,2), (2,3), (3,0),
    (4,5), (5,6), (6,7), (7,4),
    (0,4), (1,5), (2,6), (3,7),
    (3,8), (2,8), (6,8), (7,8)
)

# Faces for solid coloring
faces = (
    (0,1,2,3), (4,5,6,7), (0,1,5,4), (2,3,7,6), (1,2,6,5), (0,3,7,4), # Cube faces
    (3,2,8), (2,6,8), (6,7,8), (7,3,8) # Roof triangles
)

view_mode = 0

def draw_axes():
    glLineWidth(2)
    glBegin(GL_LINES)
    
    # X Axis (Red)
    glColor3f(1, 0, 0)
    glVertex3f(-8, 0, 0)
    glVertex3f(8, 0, 0)
    
    # Y Axis (Green)
    glColor3f(0, 1, 0)
    glVertex3f(0, -8, 0)
    glVertex3f(0, 8, 0)
    
    # Z Axis (Blue)
    glColor3f(0, 0, 1)
    glVertex3f(0, 0, -8)
    glVertex3f(0, 0, 8)
    
    glEnd()
    glLineWidth(1)

def draw_object():
    # Draw Solid Faces (Grey)
    glColor3f(0.7, 0.7, 0.7)
    
    glBegin(GL_QUADS)
    for i in range(6):
        for vertex in faces[i]:
            glVertex3fv(vertices[vertex])
    glEnd()
    
    glBegin(GL_TRIANGLES)
    for i in range(6, 10):
        for vertex in faces[i]:
            glVertex3fv(vertices[vertex])
    glEnd()
    
    # Draw Wireframe Outlines (Black)
    glColor3f(0, 0, 0)
    glLineWidth(2)
    glBegin(GL_LINES)
    for edge in edges:
        for vertex in edge:
            glVertex3fv(vertices[vertex])
    glEnd()
    glLineWidth(1)

def draw_text(x, y, text):
    glColor3f(1, 1, 1)
    glRasterPos2f(x, y)
    for char in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(char))

def display():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    
    if view_mode == 0:
        # Perspective for 3D view
        gluPerspective(45, 800.0/600.0, 0.1, 50.0)
    else:
        # Orthographic for Engineering views
        glOrtho(-8, 8, -8, 8, -20, 20)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    # Set Camera Position
    if view_mode == 0:   # 3D View
        gluLookAt(8, 6, 10, 0, 0, 0, 0, 1, 0)
    elif view_mode == 1: # Front View (Looking down Z)
        gluLookAt(0, 0, 15, 0, 0, 0, 0, 1, 0)
    elif view_mode == 2: # Top View (Looking down Y)
        gluLookAt(0, 15, 0, 0, 0, 0, 0, 0, -1)
    elif view_mode == 3: # Side View (Looking down X)
        gluLookAt(15, 0, 0, 0, 0, 0, 0, 1, 0)
    
    draw_axes()
    draw_object()
    
    # Switch to 2D Ortho to draw HUD text on top
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluOrtho2D(0, 800, 0, 600)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    view_names = ["3D VIEW", "FRONT VIEW", "TOP VIEW", "SIDE VIEW"]
    draw_text(350, 570, view_names[view_mode])
    
    glutSwapBuffers()

def keyboard(key, x, y):
    global view_mode
    
    if key == b'1':
        view_mode = 0
    elif key == b'2':
        view_mode = 1
    elif key == b'3':
        view_mode = 2
    elif key == b'4':
        view_mode = 3
    elif key == b'\x1b': # ESC key
        glutLeaveMainLoop()
    
    glutPostRedisplay()

def init():
    glEnable(GL_DEPTH_TEST)
    glClearColor(0.1, 0.1, 0.2, 1.0) # Dark Blue background

def main():
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(800, 600)
    glutCreateWindow(b"Orthographic Projection Demo")
    
    init()
    
    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    
    glutMainLoop()

if __name__ == '__main__':
    main()