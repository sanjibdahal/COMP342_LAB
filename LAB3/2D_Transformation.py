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

def translate(tx, ty):
    return [[1,0,tx],[0,1,ty],[0,0,1]]

def rotate(theta_deg):
    t = math.radians(theta_deg)
    return [[math.cos(t), -math.sin(t), 0],
            [math.sin(t),  math.cos(t), 0],
            [0,0,1]]

def scale(sx, sy):
    return [[sx,0,0],[0,sy,0],[0,0,1]]

def reflect_x():
    return [[1,0,0],[0,-1,0],[0,0,1]]

def reflect_y():
    return [[-1,0,0],[0,1,0],[0,0,1]]

def shear(shx, shy):
    return [[1,shx,0],[shy,1,0],[0,0,1]]

def draw_shape(points, filled=False):
    mode = GL_TRIANGLES if filled else GL_LINE_LOOP
    glBegin(mode)
    for x, y in points:
        glVertex2f(x, y)
    glEnd()

def draw_axes():
    glColor3f(0.5,0.5,0.5)
    glBegin(GL_LINES)
    glVertex2f(-1,0)
    glVertex2f(1,0)
    glVertex2f(0,-1)
    glVertex2f(0,1)
    glEnd()

def draw_grid():
    glColor3f(0.3,0.3,0.3)
    glBegin(GL_LINES)
    for i in range(-10,11):
        v = i * 0.1
        glVertex2f(v, -1)
        glVertex2f(v,  1)
        glVertex2f(-1, v)
        glVertex2f( 1, v)
    glEnd()

def main():
    if not glfw.init():
        return

    window = glfw.create_window(800, 600, "2D Transformations", None, None)
    glfw.make_context_current(window)
    glClearColor(0.08,0.08,0.1,1)
    glLineWidth(2)

    triangle = [[0.1,0.1],[0.1,0.5],[0.4,0.2]]
    current = [p[:] for p in triangle]

    I = [[1,0,0],[0,1,0],[0,0,1]]

    def key_callback(window,key,scancode,action,mods):
        nonlocal current
        if action == glfw.PRESS:
            if key == glfw.KEY_0:
                current = [p[:] for p in triangle]

            elif key == glfw.KEY_1:
                M = mat_mult(translate(0.2,0.2), I)
                current = apply_matrix(triangle, M)

            elif key == glfw.KEY_2:
                M = mat_mult(rotate(30), I)
                current = apply_matrix(triangle, M)

            elif key == glfw.KEY_3:
                M = mat_mult(scale(1.2,1.2), I)
                current = apply_matrix(triangle, M)

            elif key == glfw.KEY_4:
                M = mat_mult(reflect_x(), I)
                current = apply_matrix(triangle, M)

            elif key == glfw.KEY_5:
                M = mat_mult(reflect_y(), I)
                current = apply_matrix(triangle, M)

            elif key == glfw.KEY_6:
                M = mat_mult(shear(0.3,0), I)
                current = apply_matrix(triangle, M)

    glfw.set_key_callback(window, key_callback)

    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT)
        draw_grid()
        draw_axes()

        glColor3f(1,1,1)
        draw_shape(triangle, filled=True)

        glColor3f(0.2,0.8,0.9)
        draw_shape(current, filled=True)

        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()

if __name__ == "__main__":
    main()
