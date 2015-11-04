// texture.cpp

#include <GL/gl.h>
#include <GL/glu.h>

#include "texture.h"
#include "image.h"

/***************************************************************************\
 * TextureManager::get_texture                                             *
\***************************************************************************/
GLuint TextureManager::get_texture(const std::string &filename)
{
  auto it = registred_textures.find(filename);
  if (it != registred_textures.end())
    return it->second;

  Image  image(filename);
  GLuint texture;

  // Generate a texture name
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Setup texture filters
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.get_width(), image.get_height(), GL_RGB,
                     GL_UNSIGNED_BYTE, image.get_pixels());
  registred_textures[filename] = texture;
  return texture;
}

/***************************************************************************\
 * TextureManager::~TextureManager                                         *
\***************************************************************************/
TextureManager::~TextureManager()
{
  // FIXME: delete textures
}
