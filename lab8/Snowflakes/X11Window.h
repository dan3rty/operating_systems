#pragma once
#include <X11/Xlib.h>
#include <stdexcept>

constexpr int CIRCLE_DEGREE = 360 * 64;

class X11Window
{
public:
	X11Window()
	{
		m_display = XOpenDisplay(nullptr);
		if (!m_display)
		{
			throw std::runtime_error("Failed to open display.");
		}

		const Screen* screen = DefaultScreenOfDisplay(m_display);
		m_width = screen->width;
		m_height = screen->height;

		m_window = XCreateSimpleWindow(
			m_display, DefaultRootWindow(m_display), 0, 0, m_width, m_height, 0,
			BlackPixel(m_display, 0), BlackPixel(m_display, 0));

		XSelectInput(m_display, m_window, ExposureMask | KeyPressMask | StructureNotifyMask);
		XMapWindow(m_display, m_window);
	}

	void DrawCircle(const int x, const int y, const int radius) const
	{
		XFillArc(
			m_display,
			m_window,
			DefaultGC(m_display, 0),
			x,
			y,
			radius,
			radius,
			0,
			CIRCLE_DEGREE);
	}

	void DrawBackground() const
	{
		XSetForeground(m_display, DefaultGC(m_display, 0), WhitePixel(m_display, 0));
	}

	~X11Window()
	{
		if (m_display)
		{
			XDestroyWindow(m_display, m_window);
			XCloseDisplay(m_display);
		}
	}

	[[nodiscard]] Display* GetDisplay() const { return m_display; }
	[[nodiscard]] Window GetWindow() const { return m_window; }
	[[nodiscard]] int GetWidth() const { return m_width; }
	[[nodiscard]] int GetHeight() const { return m_height; }

	[[nodiscard]] bool ProcessEvents() const
	{
		XEvent event;
		while (XPending(m_display))
		{
			XNextEvent(m_display, &event);
			if (event.type == KeyPress || (event.type == ClientMessage && event.xclient.data.l[0] == XInternAtom(m_display, "WM_DELETE_WINDOW", False)))
			{
				return false;
			}
		}
		return true;
	}

	void Clear() const
	{
		XClearWindow(m_display, m_window);
	}

private:
	Display* m_display = nullptr;
	Window m_window;
	int m_width;
	int m_height;
};
