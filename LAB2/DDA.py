# DDA
import glfw
from OpenGL.GL import *
import time
import math

SCREEN_W, SCREEN_H = 1000, 600
REFRESH_RATE_MS = 3
LINE_START_PT = (100, 100)
LINE_END_PT = (900, 500)
POINT_SIZE = 4.0

pixel_buffer = []

def calculate_line_points(x_start, y_start, x_end, y_end):
    delta_x = x_end - x_start
    delta_y = y_end - y_start

    magnitude = int(max(abs(delta_x), abs(delta_y)))

    if magnitude == 0:
        yield round(x_start), round(y_start)
        return

    x_step = delta_x / magnitude
    y_step = delta_y / magnitude

    current_x = x_start
    current_y = y_start

    for _ in range(magnitude + 1):
        x_pixel = round(current_x)
        y_pixel = round(current_y)
        
        yield int(x_pixel), int(y_pixel)
        
        current_x += x_step
        current_y += y_step


def draw_frame():
    glClear(GL_COLOR_BUFFER_BIT)
    
    glPointSize(POINT_SIZE) 

    glBegin(GL_POINTS)
    
    glColor3f(0.0, 1.0, 1.0) 
    
    for px, py in pixel_buffer:
        glVertex2i(px, py)
        
    glEnd()


def setup_opengl_context():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    
    glOrtho(0.0, SCREEN_W, 0.0, SCREEN_H, -1.0, 1.0)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    glClearColor(0.2, 0.2, 0.25, 1.0)


def main_loop():
    global pixel_buffer

    if not glfw.init():
        raise RuntimeError("GLFW initialization failed.")
    
    glfw.window_hint(glfw.RESIZABLE, glfw.FALSE)
    
    window = glfw.create_window(SCREEN_W, SCREEN_H, "DDA Line Animation - v2", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("Window creation failed.")

    glfw.make_context_current(window)
    setup_opengl_context()

    line_stream = calculate_line_points(
        LINE_START_PT[0], LINE_START_PT[1], 
        LINE_END_PT[0], LINE_END_PT[1]
    )
    
    last_draw_time = time.time()
    
    while not glfw.window_should_close(window):
        glfw.poll_events()

        if (time.time() - last_draw_time) * 1000 >= REFRESH_RATE_MS:
            try:
                p_x, p_y = next(line_stream)
                pixel_buffer.append((p_x, p_y))
            except StopIteration:
                pass
            last_draw_time = time.time()

        draw_frame()
        
        glfw.swap_buffers(window)

    glfw.terminate()


if __name__ == "__main__":
    main_loop()