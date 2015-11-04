// texture.h

#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <string>

#include <GL/gl.h>

class TextureManager
{
  std::map<std::string, GLuint> registred_textures;
public:
  ~TextureManager();
  GLuint get_texture(const std::string &filename);
};

#endif
