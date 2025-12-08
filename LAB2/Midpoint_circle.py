# Midpoint Circle Algorithm
import glfw
from OpenGL.GL import *
import time

WIN_WIDTH, WIN_HEIGHT = 800, 600
CENTER_X, CENTER_Y = 400, 300 
RADIUS = 200                   
REFRESH_RATE_MS = 2         

circle_pixels = []


def draw_octants(x_center, y_center, x, y):
    
    yield (x_center + x, y_center + y)
    yield (x_center + y, y_center + x)
    yield (x_center - y, y_center + x)
    yield (x_center - x, y_center + y)
    yield (x_center - x, y_center - y)
    yield (x_center - y, y_center - x)
    yield (x_center + y, y_center - x)
    yield (x_center + x, y_center - y)


def midpoint_circle_generator(x_center, y_center, radius):
    x = 0
    y = radius
    p = 1 - radius

    yield from draw_octants(x_center, y_center, x, y)
    
    while x < y:
        if p < 0:
            # P_new = P_old + 2x + 1
            x += 1
            p += 2 * x + 1 
        else:
            # P_new = P_old + 2x - 2y + 1
            x += 1
            y -= 1
            p += 2 * x - 2 * y + 1 
        
        yield from draw_octants(x_center, y_center, x, y)


def render_pixels():
    glClear(GL_COLOR_BUFFER_BIT)
    glPointSize(4)

    glBegin(GL_POINTS)
    glColor3f(1.0, 0.0, 0.7)
    
    for px, py in circle_pixels:
        glVertex2i(px, py)
        
    glEnd()


def setup_projection():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glClearColor(0.1, 0.1, 0.1, 1.0)


def run_program():
    global circle_pixels

    if not glfw.init():
        raise RuntimeError("GLFW initialization failed.")
    
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, "Midpoint Circle Algorithm", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("GLFW window creation failed.")

    glfw.make_context_current(window)
    setup_projection()

    circle_stream = midpoint_circle_generator(CENTER_X, CENTER_Y, RADIUS)
    last_draw_time = time.time()

    while not glfw.window_should_close(window):
        glfw.poll_events()
        
        if (time.time() - last_draw_time) * 1000 >= REFRESH_RATE_MS:
            try:
                p_x, p_y = next(circle_stream)
                circle_pixels.append((p_x, p_y))
            except StopIteration:
                pass
            
            last_draw_time = time.time()
        
        render_pixels()
        glfw.swap_buffers(window)

    glfw.terminate()


if __name__ == "__main__":
    run_program()