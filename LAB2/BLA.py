# BLA
import glfw
from OpenGL.GL import *
import time

WIN_WIDTH, WIN_HEIGHT = 700, 550
plot_buffer = []

def bresenham_line(x0, y0, x1, y1):
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    x, y = x0, y0

    sx = 1 if x1 >= x0 else -1
    sy = 1 if y1 >= y0 else -1

    # Determine whether slope <1 or >=1
    if dx > dy:  # |m| < 1
        p = 2 * dy - dx
        for i in range(dx + 1):
            print(f"[{i:03}] pixel=({x:3},{y:3})  p={p:3}")
            yield x, y
            x += sx
            if p >= 0:
                y += sy
                p += 2 * (dy - dx)
            else:
                p += 2 * dy
    else:  # |m| >= 1
        p = 2 * dx - dy
        for i in range(dy + 1):
            print(f"[{i:03}] pixel=({x:3},{y:3})  p={p:3}")
            yield x, y
            y += sy
            if p >= 0:
                x += sx
                p += 2 * (dx - dy)
            else:
                p += 2 * dx


def render_pixels():
    glClear(GL_COLOR_BUFFER_BIT)
    glPointSize(6)

    glBegin(GL_POINTS)
    glColor3f(0.0, 0.3, 1.0)
    for px, py in plot_buffer:
        glVertex2i(px, py)
    glEnd()


def setup_projection():
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glClearColor(0.08, 0.08, 0.10, 1)


def run():
    global plot_buffer

    if not glfw.init():
        raise RuntimeError("GLFW init failed")
    window = glfw.create_window(WIN_WIDTH, WIN_HEIGHT, "Bresenham Line", None, None)
    if not window:
        glfw.terminate()
        raise RuntimeError("Window creation failed")

    glfw.make_context_current(window)
    setup_projection()

    print("Idx | pixel (x,y) | decision parameter p")

    stream = bresenham_line(82, 50, 400, 500)
    last = time.time()

    while not glfw.window_should_close(window):
        glfw.poll_events()

        if time.time() - last >= 0.004:
            try:
                px, py = next(stream)
                plot_buffer.append((px, py))
            except StopIteration:
                pass
            last = time.time()

        render_pixels()
        glfw.swap_buffers(window)

    glfw.terminate()


if __name__ == "__main__":
    run()