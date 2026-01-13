from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math

pyramid_vertices = (
    (0, 2, 0),
    (1, -1, 1),
    (1, -1, -1),
    (-1, -1, -1),
    (-1, -1, 1)
)

pyramid_edges = (
    (0,1), (0,2), (0,3), (0,4),
    (1,2), (2,3), (3,4), (4,1)
)

pyramid_surfaces = (
    (0,1,2), (0,2,3), (0,3,4), (0,4,1),
    (1,2,3,4)
)

transform_matrix = None
transformation_info = ""
camera_angle = 0
camera_positions = [
    (0, 5, 15),
    (15, 5, 0),
    (0, 5, -15),
    (-15, 5, 0),
    (10, 10, 10),
    (-10, 10, 10),
    (10, 10, -10),
    (-10, 10, -10)
]

def create_translation_matrix(tx, ty, tz):
    matrix = [
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        tx, ty, tz, 1
    ]
    return matrix

def create_rotation_matrix(axis, angle_degrees):
    angle = math.radians(angle_degrees)
    c = math.cos(angle)
    s = math.sin(angle)
    
    if axis.lower() == 'x':
        matrix = [
            1, 0, 0, 0,
            0, c, s, 0,
            0, -s, c, 0,
            0, 0, 0, 1
        ]
    elif axis.lower() == 'y':
        matrix = [
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        ]
    elif axis.lower() == 'z':
        matrix = [
            c, s, 0, 0,
            -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]
    
    return matrix

def create_scaling_matrix(sx, sy, sz):
    matrix = [
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1
    ]
    
    return matrix

def create_shearing_matrix(shear_axis, shear_factor, relative_axis):
    matrix = [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
    
    if shear_axis.lower() == 'x' and relative_axis.lower() == 'y':
        matrix[1] = shear_factor
    elif shear_axis.lower() == 'x' and relative_axis.lower() == 'z':
        matrix[2] = shear_factor
    elif shear_axis.lower() == 'y' and relative_axis.lower() == 'x':
        matrix[4] = shear_factor
    elif shear_axis.lower() == 'y' and relative_axis.lower() == 'z':
        matrix[6] = shear_factor
    elif shear_axis.lower() == 'z' and relative_axis.lower() == 'x':
        matrix[8] = shear_factor
    elif shear_axis.lower() == 'z' and relative_axis.lower() == 'y':
        matrix[9] = shear_factor
    
    return matrix

def draw_grid():
    glColor3f(0.25, 0.25, 0.25)
    glBegin(GL_LINES)
    for i in range(-10, 11):
        glVertex3f(i, -1, -10)
        glVertex3f(i, -1, 10)
        glVertex3f(-10, -1, i)
        glVertex3f(10, -1, i)
    glEnd()

def draw_axes():
    glLineWidth(4)
    glBegin(GL_LINES)
    
    glColor3f(1, 0, 0)
    glVertex3f(-10, 0, 0)
    glVertex3f(10, 0, 0)
    
    glColor3f(0, 1, 0)
    glVertex3f(0, -2, 0)
    glVertex3f(0, 10, 0)
    
    glColor3f(0, 0, 1)
    glVertex3f(0, 0, -10)
    glVertex3f(0, 0, 10)
    
    glEnd()
    glLineWidth(1)
    
    glColor3f(1, 1, 1)
    glRasterPos3f(10.5, 0, 0)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord('X'))
    glRasterPos3f(0, 10.5, 0)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord('Y'))
    glRasterPos3f(0, 0, 10.5)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord('Z'))

def draw_text(x, y, text):
    """Draw 2D text on screen"""
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0, 800, 0, 600)
    
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    
    glColor3f(1, 1, 1)
    glRasterPos2f(x, y)
    for char in text:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(char))
    
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)

def draw_pyramid(color):
    glColor4f(color[0], color[1], color[2], 0.8)
    
    glBegin(GL_TRIANGLES)
    for i in range(4):
        glVertex3fv(pyramid_vertices[pyramid_surfaces[i][0]])
        glVertex3fv(pyramid_vertices[pyramid_surfaces[i][1]])
        glVertex3fv(pyramid_vertices[pyramid_surfaces[i][2]])
    glEnd()
    
    glBegin(GL_QUADS)
    glVertex3fv(pyramid_vertices[pyramid_surfaces[4][0]])
    glVertex3fv(pyramid_vertices[pyramid_surfaces[4][1]])
    glVertex3fv(pyramid_vertices[pyramid_surfaces[4][2]])
    glVertex3fv(pyramid_vertices[pyramid_surfaces[4][3]])
    glEnd()
    
    glColor3f(0, 0, 0)
    glLineWidth(3)
    glBegin(GL_LINES)
    for edge in pyramid_edges:
        for vertex in edge:
            glVertex3fv(pyramid_vertices[vertex])
    glEnd()
    glLineWidth(1)

def display():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 800/600, 0.1, 50.0)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    cam_pos = camera_positions[camera_angle]
    gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], 
              0, 0, 0,
              0, 1, 0)
    
    draw_grid()
    draw_axes()
    
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    glPushMatrix()
    draw_pyramid((0.2, 0.6, 1.0))
    glPopMatrix()
    
    if transform_matrix:
        glPushMatrix()
        glMultMatrixf(transform_matrix)
        draw_pyramid((1.0, 0.3, 0.3))
        glPopMatrix()
    
    glDisable(GL_BLEND)
    
    # Draw transformation info
    if transformation_info:
        draw_text(10, 580, f"Current Transformation: {transformation_info}")
    
    draw_text(10, 30, "1: Translation | 2: Rotation | 3: Shearing | 4: Scaling")
    draw_text(10, 10, "C: Change Camera | R: Reset | ESC: Exit")
    
    glutSwapBuffers()

def keyboard(key, x, y):
    global transform_matrix, camera_angle
    
    if key == b'1':
        transform_matrix= create_translation_matrix(4, 4, 0)
    elif key == b'2':
        transform_matrix= create_rotation_matrix('x', 30)
    elif key == b'3':
        transform_matrix= create_shearing_matrix('x', 0.8, 'y')
    elif key == b'4':
        transform_matrix= create_scaling_matrix(2, 2, 2)
    elif key == b'c' or key == b'C':
        camera_angle = (camera_angle + 1) % 8
        print(f"Camera position: {camera_positions[camera_angle]}")
    elif key == b'r' or key == b'R':
        transform_matrix = None
        transformation_info = ""
        print("Reset transformation")
    elif key == b'\x1b':
        glutLeaveMainLoop()
    
    glutPostRedisplay()

def init():
    glEnable(GL_DEPTH_TEST)
    glClearColor(0.05, 0.05, 0.15, 1.0)
    glEnable(GL_POINT_SMOOTH)
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)

def main():
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(800, 600)
    glutCreateWindow(b"3D Transformations")
    
    init()
    
    print("\n=== 3D Transformations Demo ===")
    print("Press 1: Translation by (4, 4, 0)")
    print("Press 2: Rotation about Y-axis by 30 degrees")
    print("Press 3: Shearing along X-axis by factor 0.8 relative to Y-axis")
    print("Press 4: Uniform scaling by factor 2")
    print("Press C: Change camera view")
    print("Press R: Reset transformation")
    print("Press ESC: Exit\n")
    
    glutDisplayFunc(display)
    glutKeyboardFunc(keyboard)
    
    glutMainLoop()

if __name__ == '__main__':
    main()