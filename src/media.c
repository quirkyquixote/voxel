
#include "media.h"

#include <SDL2/SDL_image.h>
#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

#include "context.h"

GLuint texture(const char *path)
{
	SDL_Surface *image;
	GLuint tex;
	int fmt, w, h;
	void *data;

	image = IMG_Load(path);
	if (image == NULL) {
		log_error("%s", IMG_GetError());
		return -1;
	}
	fmt = image->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
	w = image->w;
	h = image->h;
	data = image->pixels;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	SDL_FreeSurface(image);
	return tex;
}

void texture_destroy(GLuint tex)
{
	glDeleteTextures(1, &tex);
}

