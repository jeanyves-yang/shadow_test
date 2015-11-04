// md2_player.h

#ifndef MD2_PLAYER_H
#define MD2_PLAYER_H

#include <memory>
#include <string>

#include "md2_model.h"

namespace Md2
{
  class Player
  {
    typedef std::unique_ptr<Model> ModelPtr;

    ModelPtr player_mesh;

    Object player_object;

    std::string name;
    std::string current_skin;
    std::string current_anim;
  public:
    Player(const std::string &dirname) throw(std::runtime_error);

    void draw_player_itp(bool animated);
    void animate(float percent);

    // Setters and accessors
    void set_scale(GLfloat scale);
    void set_skin(const std::string &name);
    void set_anim(const std::string &name);

    const std::string &get_current_skin() const { return current_skin; }
    const std::string &get_current_anim() const { return current_anim; }

    const Model *get_player_mesh() const { return player_mesh.get(); }
  };
}
#endif
