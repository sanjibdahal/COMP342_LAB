import glfw
from OpenGL.GL import *

scale_x, scale_y = 1.0, 1.0


def resize_callback(window, w, h):
    global scale_x, scale_y
    glViewport(0, 0, w, h)
    scale_x = 2.0 / w
    scale_y = 2.0 / h


def plot(x, y):
    glVertex2f(x * scale_x, y * scale_y)


def ellipse_points(cx, cy, x, y):
    plot(cx + x, cy + y)
    plot(cx - x, cy + y)
    plot(cx + x, cy - y)
    plot(cx - x, cy - y)


def draw_midpoint_ellipse(cx, cy, rx, ry):
    x, y = 0, ry
    rx2, ry2 = rx * rx, ry * ry

    glBegin(GL_POINTS)

    # First Zone
    d = ry2 - rx2 * ry + 0.25 * rx2

    while ry2 * x <= rx2 * y:
        ellipse_points(cx, cy, x, y)

        if d < 0:
            x += 1
            d += ry2 * (2 * x + 1)
        else:
            x += 1
            y -= 1
            d += ry2 * (2 * x + 1) - 2 * rx2 * y

    # Second Zone
    d = (ry2 * (x + 0.5) ** 2) + (rx2 * (y - 1) ** 2) - rx2 * ry2

    while y >= 0:
        ellipse_points(cx, cy, x, y)

        if d > 0:
            y -= 1
            d += rx2 * (1 - 2 * y)
        else:
            x += 1
            y -= 1
            d += ry2 * (2 * x) + rx2 * (1 - 2 * y)

    glEnd()


def main():
    if not glfw.init():
        return

    window = glfw.create_window(800, 600, "Midpoint Ellipse", None, None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)
    glfw.set_framebuffer_size_callback(window, resize_callback)

    w, h = glfw.get_framebuffer_size(window)
    resize_callback(window, w, h)

    glClearColor(0.08, 0.08, 0.1, 1)
    glPointSize(3.5)

    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT)

        glColor3f(0.2, 0.7, 0.9)
        draw_midpoint_ellipse(0, 0, 220, 150)

        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()


if __name__ == "__main__":
    main()

