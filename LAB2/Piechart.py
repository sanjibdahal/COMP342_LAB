# Pie Chart
import glfw
from OpenGL.GL import *
import math
import time

WIN_WIDTH, WIN_HEIGHT = 800, 600
CENTER_X, CENTER_Y = 400, 300 
RADIUS = 200                    
NUM_SEGMENTS = 100              
ANIMATION_DURATION_S = 1.0      

PIE_DATA = [
    (30, 1.0, 0.2, 0.2), 
    (25, 0.2, 0.8, 0.2),  
    (15, 0.2, 0.2, 1.0),  
    (30, 1.0, 0.8, 0.0)   
]

current_slice_index = 0         
slice_start_time = 0.0          
total_value = sum(item[0] for item in PIE_DATA)
animation_finished = False


def draw_pie_chart(data, center_x, center_y, radius, current_index, animation_factor):
    current_angle = 0.0 
    
    for index, (value, r, g, b) in enumerate(data):
        slice_angle = (value / total_value) * 2 * math.pi
        
        if index < current_index:
            draw_angle = slice_angle
            factor = 1.0
        elif index == current_index:
            draw_angle = slice_angle * animation_factor 
            factor = animation_factor
        else:
            break
            
        end_angle = current_angle + draw_angle
        
        glColor3f(r, g, b)
        
        glBegin(GL_TRIANGLE_FAN)
        glVertex2f(center_x, center_y)
        
        for i in range(NUM_SEGMENTS + 1):
            progress_ratio = (i / NUM_SEGMENTS)
            
            if progress_ratio > factor:
                break
            
            angle = current_angle + progress_ratio * draw_angle
            
            x = center_x + radius * math.cos(angle)
            y = center_y + radius * math.sin(angle)
            
            glVertex2f(x, y)
            
        glEnd()
        
        current_angle += slice_angle


def render_scene(factor):
    glClear(GL_COLOR_BUFFER_BIT)
    
    draw_pie_chart(PIE_DATA, CENTER_X, CENTER_Y, RADIUS, current_slice_index, factor)
    
    glfw.swap_buffers(window)

def setup_projection(width, height):
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, width, 0, height, -1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def run_program():
    global window, current_slice_index, slice_start_time, animation_finished
    
    if not glfw.init():
        raise RuntimeError("GLFW initialization failed.")
    
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, "Pie Chart", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("GLFW window creation failed.")

    glfw.make_context_current(window)
    setup_projection(WIN_WIDTH, WIN_HEIGHT)

    slice_start_time = time.time()
    
    while not glfw.window_should_close(window):
        glfw.poll_events()
        
        current_time = time.time()
        elapsed_time = current_time - slice_start_time
        
        if ANIMATION_DURATION_S > 0:
             factor = min(1.0, elapsed_time / ANIMATION_DURATION_S)
        else:
             factor = 1.0

        if not animation_finished and factor >= 1.0:
            current_slice_index += 1
            slice_start_time = current_time 
            
            if current_slice_index >= len(PIE_DATA):
                animation_finished = True
                current_slice_index = len(PIE_DATA) - 1
                
        if animation_finished:
             factor = 1.0
             
        render_scene(factor)

    glfw.terminate()


if __name__ == "__main__":
    run_program()