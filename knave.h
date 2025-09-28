#if !defined(KNAVE_CORE_H)
#define KNAVE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void knave_error(const char *format, ...);
void knave_warning(const char *format, ...);
void knave_info(const char *format, ...);
void knave_debug(const char *format, ...);

typedef struct Knave_Window Knave_Window;
Knave_Window *knave_window_create(void);
void knave_window_destroy(Knave_Window *window);
void knave_window_update(Knave_Window *window);
void knave_window_refresh(Knave_Window *window);
void knave_window_present(Knave_Window *window);
bool knave_window_should_close(const Knave_Window *window);
void knave_window_should_close_set(Knave_Window *window, bool should_close);
const char *knave_window_title(const Knave_Window *window);
void knave_window_title_set(Knave_Window *window, const char *title);
void knave_window_dimensions(const Knave_Window *window, uint32_t *out_width, uint32_t *out_height);
void knave_window_dimensions_set(Knave_Window *window, uint32_t width, uint32_t height);
void knave_window_screen_dimensions(Knave_Window *window, uint32_t *out_width, uint32_t *out_height);
void knave_window_position(const Knave_Window *window, int32_t *out_x, int32_t *out_y);
void knave_window_position_set(Knave_Window *window, int32_t x, int32_t y);
void knave_window_move_to_center(Knave_Window *window);
bool knave_window_focus(const Knave_Window *window);
void knave_window_focus_set(Knave_Window *window, bool focus);

#endif /* KNAVE_H */

#if defined(KNAVE_IMPLEMENTATION)
#undef KNAVE_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_PRINT_FUNCTION(NAME, PREFIX, STREAM) \
	void NAME(const char *format, ...) \
	{ \
		va_list args; \
		fprintf(STREAM, PREFIX ": "); \
		va_start(args, format); \
		vfprintf(STREAM, format, args); \
		va_end(args); \
		fputc('\n', STREAM); \
	}
#define DEFINE_PRINT_FUNCTION_STUB(NAME) \
	void NAME(const char *format, ...) \
	{ \
		(void)format; \
	}

#if defined(KNAVE_ERROR_DISABLED)
DEFINE_PRINT_FUNCTION_STUB(knave_error)
#else
DEFINE_PRINT_FUNCTION(knave_error, "ERROR", stderr)
#endif

#if defined(KNAVE_WARNING_DISABLED)
DEFINE_PRINT_FUNCTION_STUB(knave_warning)
#else
DEFINE_PRINT_FUNCTION(knave_warning, "WARNING", stderr)
#endif

#if defined(KNAVE_INFO_DISABLED)
DEFINE_PRINT_FUNCTION_STUB(knave_info)
#else
DEFINE_PRINT_FUNCTION(knave_info, "INFO", stdout)
#endif

#if defined(KNAVE_DEBUG_DISABLED)
DEFINE_PRINT_FUNCTION_STUB(knave_debug)
#else
DEFINE_PRINT_FUNCTION(knave_debug, "DEBUG", stdout)
#endif

#undef DEFINE_PRINT_FUNCTION
#undef DEFINE_PRINT_FUNCTION_STUB

static char *k_strdup(const char *string)
{
	size_t i, length;
	char *data;

	for (length = 0; string[length] != '\0'; ++length) {}

	data = malloc(length + 1);
	if (data == NULL) return NULL;

	for (i = 0; i <= length; ++i) {
		data[i] = string[i];
	}
	
	return data;
}

#if defined(__linux__)

#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>

struct Knave_Window {
	Display *display;
	GLXContext context;
	char *title;
	Colormap colormap;
	Window window;
	Atom close_message;
	bool should_close;
};

static int k_check_glx_version(Display *display);
static GLXFBConfig k_fbconfig_find(Display *display, int screen);
static void k_screen_dimensions(Display *display, int screen, uint32_t *out_width, uint32_t *out_height);
static GLXContext k_setup_glx_context(Display *display, GLXFBConfig fbconfig, Window window);
static Bool k_window_event_predicate(Display *display, XEvent *event, XPointer arg);
static void k_window_event_dispatch(Knave_Window *window, const XEvent *event);

Knave_Window *knave_window_create(void)
{
	Knave_Window *window = NULL;
	int screen;
	GLXFBConfig fbconfig;
	XVisualInfo *visual = NULL;
	Window root;
	uint32_t screen_width, screen_height, width, height;
	int x, y, border_width, io_class, attribs_mask;
	XSetWindowAttributes attribs;

	window = malloc(sizeof *window);
	if (window == NULL) {
		knave_error("Failed to allocate %lu bytes for the window.", sizeof *window);
		goto on_failure;
	}

	window->display = XOpenDisplay(NULL);
	if (window->display == NULL) {
		knave_error("Failed to open the X11 display.");
		goto on_failure;
	}

	k_check_glx_version(window->display);

	screen = DefaultScreen(window->display);
	fbconfig = k_fbconfig_find(window->display, screen);
	if (fbconfig == NULL) goto on_failure;
	visual = glXGetVisualFromFBConfig(window->display, fbconfig);
	assert(visual != NULL);

	root = RootWindow(window->display, screen);
	window->colormap = XCreateColormap(window->display, root, visual->visual, AllocNone);
	if (window->colormap == 0) {
		knave_error("Failed to create the X11 colormap.");
		goto on_failure;
	}

	k_screen_dimensions(window->display, screen, &screen_width, &screen_height);
	width = (int)((float)screen_width * 2.0f / 3.0f);
	height = (int)((float)screen_height * 2.0f / 3.0f);
	x = (int)((float)(screen_width - width) / 2.0f);
	y = (int)((float)(screen_height - height) / 2.0f);

	border_width = 0;
	io_class = InputOutput;
	attribs_mask = CWBorderPixel | CWColormap | CWEventMask;
	attribs.border_pixel = BlackPixel(window->display, screen);
	attribs.colormap = window->colormap;
	attribs.event_mask = StructureNotifyMask;

	window->window = XCreateWindow(window->display, root, x, y, width, height, border_width, visual->depth, io_class, visual->visual, attribs_mask, &attribs);
	if (window->window == 0) {
		knave_error("Failed to create the X11 window.");
		goto on_failure;
	}

	window->close_message = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
	if (!XSetWMProtocols(window->display, window->window, &window->close_message, 1)) {
		knave_warning("Failed to set the WM protocol for closing the window.");
	}

	window->context = k_setup_glx_context(window->display, fbconfig, window->window);
	if (window->context == NULL) goto on_failure;

	window->title = k_strdup("Knave");
	XStoreName(window->display, window->window, window->title);
	XMapWindow(window->display, window->window);
	XMoveWindow(window->display, window->window, x, y);
	
	XFree(visual);
	return window;

on_failure:
	if (visual != NULL) {
		XFree(visual);
	}
	knave_window_destroy(window);
	return NULL;
}

void knave_window_destroy(Knave_Window *window)
{
	if (window == NULL) return;
	if (window->display != NULL) {
		if (window->context != NULL) {
			glXMakeCurrent(window->display, None, NULL);
			glXDestroyContext(window->display, window->context);
		}
		if (window->colormap != 0) {
			XFreeColormap(window->display, window->colormap);
		}
		if (window->window != 0) {
			XDestroyWindow(window->display, window->window);
		}
		XCloseDisplay(window->display);
	}
}

void knave_window_update(Knave_Window *window)
{
	static const int Max_Events = 16;
	int i;

	for (i = 0; i < Max_Events; ++i) {
		XEvent event;
		
		if (!XPending(window->display)) break;
		if (XCheckIfEvent(window->display, &event, k_window_event_predicate, (XPointer)window)) {
			k_window_event_dispatch(window, &event);
		}
	}
}

void knave_window_refresh(Knave_Window *window)
{
	glXMakeCurrent(window->display, window->window, window->context);
	glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void knave_window_present(Knave_Window *window)
{
	glXSwapBuffers(window->display, window->window);
}

bool knave_window_should_close(const Knave_Window *window)
{
	return window->should_close;
}

void knave_window_should_close_set(Knave_Window *window, bool should_close)
{
	window->should_close = should_close;
}

const char *knave_window_title(const Knave_Window *window)
{
	return window->title;
}

void knave_window_title_set(Knave_Window *window, const char *title)
{
	if (window->title != NULL) {
		free(window->title);
	}
	window->title = k_strdup(title);
	XStoreName(window->display, window->window, window->title);
	XMapWindow(window->display, window->window);
}

void knave_window_dimensions(const Knave_Window *window, uint32_t *out_width, uint32_t *out_height)
{
	Window root;
	int x, y;
	unsigned int border, depth;
	XGetGeometry(window->display, window->window, &root, &x, &y, out_width, out_height, &border, &depth);
}

void knave_window_dimensions_set(Knave_Window *window, uint32_t width, uint32_t height)
{
	XResizeWindow(window->display, window->window, width, height);
}

void knave_window_screen_dimensions(Knave_Window *window, uint32_t *out_width, uint32_t *out_height)
{
	k_screen_dimensions(window->display, DefaultScreen(window->display), out_width, out_height);
}

void knave_window_position(const Knave_Window *window, int32_t *out_x, int32_t *out_y)
{
	Window root;
	unsigned int width, height, border, depth;
	XGetGeometry(window->display, window->window, &root, out_x, out_y, &width, &height, &border, &depth);
}

void knave_window_position_set(Knave_Window *window, int32_t x, int32_t y)
{
	XMoveWindow(window->display, window->window, x, y);
}

void knave_window_move_to_center(Knave_Window *window)
{
	uint32_t screen_width, screen_height, width, height;
	int32_t x, y;

	knave_window_screen_dimensions(window, &screen_width, &screen_height);
	knave_window_dimensions(window, &width, &height);
	x = (int32_t)(((float)screen_width - (float)width) / 2.0f);
	y = (int32_t)(((float)screen_height - (float)height) / 2.0f);

	knave_window_position_set(window, x, y);
}

bool knave_window_focus(const Knave_Window *window)
{
	Window focused;
	int revert;

	XGetInputFocus(window->display, &focused, &revert);
	return window->window == focused;
}

void knave_window_focus_set(Knave_Window *window, bool focus)
{
	Window target = focus ? window->window : None;
	XSetInputFocus(window->display, target, RevertToParent, CurrentTime);
}

static int k_check_glx_version(Display *display)
{
	int major, minor;
	static const int Min_Major = 1, Min_Minor = 3;

	if (!glXQueryVersion(display, &major, &minor)) {
		knave_warning("Failed to query the GLX version.");
		return -1;
	}

	if (major < Min_Major || (minor < Min_Minor && major == Min_Major)) {
		knave_warning("Current GLX is %d.%d, but version %d.%d or greater is required.", major, minor, Min_Major, Min_Minor);
		return -1;
	}

	return 0;
}

static GLXFBConfig k_fbconfig_find(Display *display, int screen)
{
	static const int Attribs[] = {
		GLX_FBCONFIG_ID, GLX_DONT_CARE,
		GLX_BUFFER_SIZE, 0,
		GLX_LEVEL, 0,
		GLX_DOUBLEBUFFER, True,
		GLX_STEREO, False,
		GLX_AUX_BUFFERS, 0,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_ACCUM_RED_SIZE, 0,
		GLX_ACCUM_GREEN_SIZE, 0,
		GLX_ACCUM_BLUE_SIZE, 0,
		GLX_ACCUM_ALPHA_SIZE, 0,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_X_RENDERABLE, True,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_CONFIG_CAVEAT, GLX_NONE,
		GLX_TRANSPARENT_INDEX_VALUE, 0,
		GLX_TRANSPARENT_RED_VALUE, 0,
		GLX_TRANSPARENT_GREEN_VALUE, 0,
		GLX_TRANSPARENT_BLUE_VALUE, 0,
		GLX_TRANSPARENT_ALPHA_VALUE, 0,
		None};
	
	GLXFBConfig *options, best;
	int count, i, best_samples;

	options = glXChooseFBConfig(display, screen, Attribs, &count);
	if (options == NULL || count < 1) {
		knave_error("Failed to retrieve any GLX framebuffer configuration.");
		return NULL;
	}

	best = NULL;
	best_samples = -1;
	for (i = 0; i < count; ++i) {
		GLXFBConfig config;
		XVisualInfo *visual;
		int samples, buffers;

		config = options[i];
		visual = glXGetVisualFromFBConfig(display, config);
		if (visual == NULL) continue;
		XFree(visual);

		if (glXGetFBConfigAttrib(display, config, GLX_SAMPLES, &samples) != 0) continue;
		if (glXGetFBConfigAttrib(display, config, GLX_SAMPLE_BUFFERS, &buffers) != 0) continue;

		if (samples > best_samples && buffers > 0) {
			best = config;
			best_samples = samples;
		}
	}
	
	if (best == NULL) {
		knave_error("Could not find a compatible GLX framebuffer configuration in %d available options.", count);
	}
	XFree(options);
	return best;
}

static int k_xinerama_screen_dimensions(Display *display, uint32_t *out_width, uint32_t *out_height)
{
	int count;
	XineramaScreenInfo *infos;

	if (!XineramaIsActive(display)) return -1;

	infos = XineramaQueryScreens(display, &count);
	if (infos == NULL || count < 1) {
		knave_warning("Could not retrieve the Xinerama screen information.");
		return -1;
	}

	*out_width = infos->width;
	*out_height = infos->height;
	return 0;
}

static void k_screen_dimensions(Display *display, int screen, uint32_t *out_width, uint32_t *out_height)
{
	Screen *info;

	if (k_xinerama_screen_dimensions(display, out_width, out_height) == 0) return;

	info = ScreenOfDisplay(display, screen);
	*out_width = WidthOfScreen(info);
	*out_height = HeightOfScreen(info);
}

static GLXContext k_setup_glx_context_legacy(Display *display, GLXFBConfig fbconfig, Window window)
{
	GLXContext context;
	XVisualInfo *visual;

	visual = glXGetVisualFromFBConfig(display, fbconfig);
	context = glXCreateContext(display, visual, NULL, True);
	XFree(visual);

	if (context == NULL) {
		knave_error("Failed to create the legacy OpenGL context.");
		return NULL;
	}

	if (!glXMakeCurrent(display, window, context)) {
		knave_error("Failed to make the legacy OpenGL context current.");
		glXDestroyContext(display, context);
		return NULL;
	}

	return context;
}

static GLXContext k_setup_glx_context(Display *display, GLXFBConfig fbconfig, Window window)
{
	typedef GLXContext (*Context_Fn)(Display *display, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list);

	static const int Attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 6,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None};

	Context_Fn glXCreateContextAttribsARB;
	GLXContext context;

	glXCreateContextAttribsARB = (Context_Fn)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == NULL) {
		knave_error("Failed to retrieve the function for creating a modern OpenGL context.");
		return k_setup_glx_context_legacy(display, fbconfig, window);
	}

	context = glXCreateContextAttribsARB(display, fbconfig, NULL, True, Attribs);
	if (context == NULL) {
		knave_error("Failed to create the modern OpenGL context.");
		return k_setup_glx_context_legacy(display, fbconfig, window);
	}

	if (!glXMakeCurrent(display, window, context)) {
		knave_error("Failed to make the modern OpenGL context current.");
		glXDestroyContext(display, context);
		return k_setup_glx_context_legacy(display, fbconfig, window);
	}

	return context;
}

static Bool k_window_event_predicate(Display *display, XEvent *event, XPointer arg)
{
	Knave_Window *window;
	(void)display;

	window = (Knave_Window *)arg;
	return event->xany.display == window->display && event->xany.window == window->window;
}

static void k_window_event_dispatch(Knave_Window *window, const XEvent *event)
{
	switch (event->type) {
		Atom message;

	case ClientMessage:
		message = (Atom)event->xclient.data.l[0];
		if (message == window->close_message) {
			window->should_close = true;
		}
		break;

	case ConfigureNotify:
		glViewport(0, 0, event->xconfigure.width, event->xconfigure.height);
		break;
	}
}

#endif /* __linux__ */
#endif /* KNAVE_IMPLEMENTATION */
