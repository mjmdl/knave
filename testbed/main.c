#include <stdlib.h>

#include <knave.h>
#include <knave_opengl.h>

int main(void)
{
	Knave_Window *window = knave_window_create();
	if (window == NULL) return EXIT_FAILURE;
	if (knave_opengl_library_load() != 0) {
		knave_window_destroy(window);
		return EXIT_FAILURE;
	}

	while (!knave_window_should_close(window)) {
		knave_window_update(window);
		knave_window_refresh(window);
		knave_window_present(window);
	}
	
	knave_window_destroy(window);
	return EXIT_SUCCESS;
}
