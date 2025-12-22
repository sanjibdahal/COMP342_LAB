import glfw
from OpenGL.GL import *
import math

def mat_mult(a, b):
    r = [[0,0,0],[0,0,0],[0,0,0]]
    for i in range(3):
        for j in range(3):
            for k in range(3):
                r[i][j] += a[i][k] * b[k][j]
    return r

def apply_matrix(points, m):
    out = []
    for x, y in points:
        v = [x, y, 1]
        r = [0,0,0]
        for i in range(3):
            for j in range(3):
                r[i] += m[i][j] * v[j]
        out.append([r[0], r[1]])
    return out

def reflect_x():
    return [[1,0,0],[0,-1,0],[0,0,1]]

def translate(tx, ty):
    return [[1,0,tx],[0,1,ty],[0,0,1]]

def rotate(theta):
    t = math.radians(theta)
    return [[math.cos(t), -math.sin(t), 0],
            [math.sin(t),  math.cos(t), 0],
            [0,0,1]]

def shear(shx, shy):
    return [[1,shx,0],[shy,1,0],[0,0,1]]

def draw_shape(points, color):
    glColor3f(*color)
    glBegin(GL_TRIANGLES)
    for x,y in points:
        glVertex2f(x,y)
    glEnd()

def draw_axes():
    glColor3f(0.6,0.6,0.6)
    glBegin(GL_LINES)
    glVertex2f(-1,0)
    glVertex2f(1,0)
    glVertex2f(0,-1)
    glVertex2f(0,1)
    glEnd()

def draw_grid():
    glColor3f(0.25,0.25,0.25)
    glBegin(GL_LINES)
    for i in range(-10,11):
        v = i * 0.1
        glVertex2f(v,-1)
        glVertex2f(v,1)
        glVertex2f(-1,v)
        glVertex2f(1,v)
    glEnd()

def main():
    if not glfw.init():
        return

    window = glfw.create_window(900,700,"Composite Transformation (No Overlap)",None,None)
    glfw.make_context_current(window)
    glClearColor(0.07,0.07,0.1,1)

    triangle = [
        [-0.8,-0.6],
        [-0.6,-0.6],
        [-0.7,-0.4]
    ]

    I = [[1,0,0],[0,1,0],[0,0,1]]

    M1 = mat_mult(reflect_x(), I)
    M2 = mat_mult(translate(1.2,0), M1)
    M3 = mat_mult(rotate(-45), M2)
    M4 = mat_mult(shear(0.4,0), M3)

    shapes = [
        triangle,
        apply_matrix(triangle, M1),
        apply_matrix(triangle, M2),
        apply_matrix(triangle, M3),
        apply_matrix(triangle, M4)
    ]

    colors = [
        (1.0,1.0,1.0),
        (0.3,0.6,1.0),
        (0.3,0.9,0.6),
        (0.9,0.8,0.3),
        (1.0,0.4,0.4)
    ]

    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT)
        draw_grid()
        draw_axes()

        for s,c in zip(shapes, colors):
            draw_shape(s,c)

        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()

if __name__ == "__main__":
    main()
