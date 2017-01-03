
#include "media.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

GLuint texture(const char *path)
{
	SDL_Surface *image;
	GLuint tex;
	int fmt, w, h;
	void *data;

	image = IMG_Load(path);
	if (image == NULL) {
		fprintf(stderr, "%s: %s\n", path, IMG_GetError());
		return -1;
	}
	fmt = image->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	w = image->w;
	h = image->h;
	data = image->pixels;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	SDL_FreeSurface(image);
	return tex;
}

void texture_destroy(GLuint tex)
{
	glDeleteTextures(1, &tex);
}

