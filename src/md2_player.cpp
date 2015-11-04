// md2_player.cpp

#include <fstream>
#include <iostream>

#include <dirent.h>
#include <sys/types.h>

#include <GL/gl.h>

#include "md2_player.h"
#include "texture.h"

/***************************************************************************\
 * Md2::Player::Player                                                     *
\***************************************************************************/
Md2::Player::Player(const std::string &dirname) throw (std::runtime_error)
: player_mesh(nullptr)
{
  std::ifstream ifs;
  std::string path;
  dirent *dit;
  DIR *dd;

  // Open the directory
  dd = opendir (dirname.c_str());
  if (!dd)
    throw std::runtime_error("Couldn't open dir");

  // Test if player mesh exists
  path = dirname + "/tris.md2";
  ifs.open(path.c_str(), std::ios::binary);

  if (!ifs.fail())
  {
    ifs.close();
    player_mesh = ModelPtr(new Model(path));
  }

  // If we haven't found any model, this is not a success...
  if (!player_mesh.get()) // FIXME: redondant!
    throw std::runtime_error("No model found");

  name.assign(dirname, dirname.find_last_of('/') + 1, dirname.length());

  // Read directory for textures
  while ((dit = readdir(dd)) != nullptr)
  {
    const std::string filename(dit->d_name);
    path = dirname + "/" + filename;

    const char *str = filename.c_str();
    std::string::size_type l = filename.find_last_of('.');

    // Skip directories
    if (l > filename.length())
      continue;

    // Skip files beginning with "<char>_" and files
    // ending with "_i.<char*>"
    if ((str[1] != '_') &&
        !((str[l-1] == 'i') && (str[l-2] == '_')))
    {
      if (filename.compare (l, 4, ".pcx") == 0)
        player_mesh->load_texture(path);
    }
  }

  // Close directory
  closedir(dd);

  // Attach models to MD2 objects
  if (player_mesh.get())
  {
    player_object.set_model(player_mesh.get());

    // Set first skin as default skin
    current_skin = player_mesh->get_skins().begin()->first;
    current_anim = player_object.get_current_anim();
  }
}

/***************************************************************************\
 * Md2::Player::draw_player_itp                                            *
 * Draw player objects with interpolation.                                 *
\***************************************************************************/
void Md2::Player::draw_player_itp(bool animated)
{
  player_mesh->set_texture(current_skin);
  player_object.draw_object_itp(animated);
}

/***************************************************************************\
 * Md2::Player::animate                                                    *
 * Animate player objects.                                                 *
\***************************************************************************/
void Md2::Player::animate(GLfloat percent)
{
  player_object.animate(percent);
}

/***************************************************************************\
 * Md2::Player::set_scale                                                  *
 * Scale model objects.                                                    *
\***************************************************************************/
void Md2::Player::set_scale(GLfloat scale)
{
  player_object.set_scale(scale);
}

/***************************************************************************\
 * Md2::Player::set_skin                                                   *
 * Set player skin.                                                        *
\***************************************************************************/
void Md2::Player::set_skin(const std::string &name)
{
  current_skin = name;
}

/***************************************************************************\
 * Md2::Player::set_anim                                                   *
 * Set current player animation.                                           *
\***************************************************************************/
void Md2::Player::set_anim(const std::string &name)
{
  player_object.set_anim(name);
  current_anim = name;
}
