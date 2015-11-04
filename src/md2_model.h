// md2_model.h

#ifndef MD2MODEL_H
#define MD2MODEL_H

#include <map>
#include <string>
#include <vector>

#include "texture.h"
#include "vec3.h"

namespace Md2
{
  // Md2 header
  struct Header
  {
    int ident;          // Magic number, "IDP2"
    int version;        // Md2 format version, should be 8

    int skinwidth;      // Texture width
    int skinheight;     // Texture height

    int framesize;      // Size of a frame, in bytes

    int num_skins;      // Number of skins
    int num_vertices;   // Number of vertices per frame
    int num_st;         // Number of texture coords
    int num_tris;       // Number of triangles
    int num_glcmds;     // Number of OpenGL commands
    int num_frames;     // Number of frames

    int offset_skins;   // offset to skin data
    int offset_st;      // offset to texture coords
    int offset_tris;    // offset to triangle data
    int offset_frames;  // offset to frame data
    int offset_glcmds;  // offset to OpenGL commands
    int offset_end;     // offset to the end of the file
  };

  // Skin data
  struct Skin
  {
    char name[64];  // Texture's filename
  };

  // Texture coords.
  struct TexCoord
  {
    short s;
    short t;
  };

  // Triangle data
  struct Triangle
  {
    unsigned short vertex[3];  // Triangle's vertex indices
    unsigned short st[3];      // Texture coords. indices
  };

  // Compressed Vertex data
  struct CompressedVertex
  {
    unsigned char v[3];         // Compressed vertex position
    unsigned char normalIndex;  // Normal vector index
  };

  // Frame data
  struct Frame
  {
    vec3 scale;        // Scale factors
    vec3 translate;    // Translation vector
    char name[16];     // Frame name
    std::vector<vec3> verts;
  };

  // Animation infos
  struct Anim
  {
    int start;  // first frame index
    int end;    // last frame index
  };

  /////////////////////////////////////////////////////////////////////////////
  //
  // class Md2Model -- MD2 Model Data Class.
  //
  /////////////////////////////////////////////////////////////////////////////

  class Model
  {
    // Constants
    static int  IDENT;
    static int  VERSION;

    // Model data
    Header header;
    std::vector<Skin>     skins;
    std::vector<TexCoord> texCoords;
    std::vector<Triangle> triangles;
    std::vector<Frame>    frames;

    GLfloat  scale;
    GLuint tex;
    TextureManager texture_manager;
    void setup_animations();
  public:
    typedef std::map<std::string, GLuint> SkinMap;
    typedef std::map<std::string, Anim> AnimMap;
  private:
    SkinMap skin_ids;
    AnimMap anims;
  public:
    Model(const std::string &filename);

    bool load_texture(const std::string &filename);
    void set_texture(const std::string &filename);

    void render_frame(int frame);
    void draw_model(int frameA, int frameB, float interp);

    void set_scale(GLfloat s) { scale = s; }

    // Accessors
    const SkinMap &get_skins() const { return skin_ids; }
    const AnimMap &get_anims() const { return anims; }
  };

  class Object
  {
    Model *model;

    int current_frame;
    int next_frame;
    float interp;

    float percent;
    float scale;

    // Animation data
    const Anim *anim_info;
    std::string current_anim;
    void animate(int start_frame, int end_frame, float percent);
  public:
    Object();

    void draw_object_itp(bool animated);
    void animate(float percent);

    void set_model(Model *model);
    void set_scale(float s) { scale = s; }
    void set_anim(const std::string &name);

    // Accessors
    const std::string &get_current_anim() const { return current_anim; }
  };
}

#endif
