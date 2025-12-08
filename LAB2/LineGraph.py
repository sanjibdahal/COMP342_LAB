# Line Graph using DDA Algorithm
import glfw
from OpenGL.GL import *
import time

WIN_WIDTH, WIN_HEIGHT = 900, 600
NUM_POINTS = 11                 
PADDING = 80                    
REFRESH_RATE_MS = 3            

STATIC_Y_PATTERN = [
    30, 45, 45, 65, 60, 80, 70, 95, 75, 85, 40 
]

pixel_buffer = []               
graph_stream = None             
STATIC_GRAPH_POINTS = []


def dda_line(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1

    steps = int(max(abs(dx), abs(dy)))

    if steps == 0:
        yield round(x1), round(y1)
        return

    x_inc = dx / steps
    y_inc = dy / steps

    x = x1
    y = y1

    for _ in range(steps + 1):
        yield int(round(x)), int(round(y))
        x += x_inc
        y += y_inc

def generate_static_graph_data(y_values, width, height, padding):
    points = []
    
    min_x, max_x = padding, width - padding
    min_y, max_y = padding, height - padding
    
    x_step = (max_x - min_x) / (len(y_values) - 1)
    
    y_range = 100
    
    for i in range(len(y_values)):
        x_coord = min_x + i * x_step
    
        y_scaled = (y_values[i] / y_range) * (max_y - min_y)
        y_coord = min_y + y_scaled
        
        points.append((int(x_coord), int(y_coord)))
        
    return points


def assemble_graph_stream(data_points):
    if len(data_points) < 2:
        return
        
    p1_x, p1_y = data_points[0]
    
    for i in range(1, len(data_points)):
        p2_x, p2_y = data_points[i]
        
        yield from dda_line(p1_x, p1_y, p2_x, p2_y)
            
        p1_x, p1_y = p2_x, p2_y

def render_pixels(data_points):
    glClear(GL_COLOR_BUFFER_BIT)
    line_color = (0.9, 0.3, 0.2) 
    
    # Draw the DDA lines 
    glPointSize(4) 
    glBegin(GL_POINTS)
    glColor3f(*line_color)
    for px, py in pixel_buffer:
        glVertex2i(px, py)
    glEnd()
    

def setup_projection():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def run_program():
    global pixel_buffer, graph_stream, STATIC_GRAPH_POINTS
    
    STATIC_GRAPH_POINTS = generate_static_graph_data(STATIC_Y_PATTERN, WIN_WIDTH, WIN_HEIGHT, PADDING)

    graph_stream = assemble_graph_stream(STATIC_GRAPH_POINTS)

    if not glfw.init():
        raise RuntimeError("GLFW initialization failed.")
    
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, "DDA Line Graph", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("GLFW window creation failed.")

    glfw.make_context_current(window)
    setup_projection()

    last_draw_time = time.time()

    while not glfw.window_should_close(window):
        glfw.poll_events()
        
        if (time.time() - last_draw_time) * 1000 >= REFRESH_RATE_MS:
            try:
                p_x, p_y = next(graph_stream)
                pixel_buffer.append((p_x, p_y))
            except StopIteration:
                pass
            
            last_draw_time = time.time()
        
        render_pixels(STATIC_GRAPH_POINTS)
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    run_program()