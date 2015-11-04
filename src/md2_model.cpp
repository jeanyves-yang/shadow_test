// md2_model.cpp

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glut.h>

#include "md2_model.h"
#include "vec2.h"
#include "matrix.h"

int Md2::Model::IDENT = 'I' + ('D'<<8) + ('P'<<16) + ('2'<<24);
int Md2::Model::VERSION = 8;

/***************************************************************************\
 * Md2::Model::Model                                                       *
\***************************************************************************/
Md2::Model::Model(const std::string &filename) : scale(1), tex(0)
{
  // ouverture du fichier
  std::ifstream ifs(filename.c_str(), std::ios::binary);

  if (ifs.fail())
  {
    std::cerr << "Ouverture du fichier " + filename + " impossible\n";
    exit(-1);
  }

  // Lecture de l'entête du fichier
  ifs.read(reinterpret_cast<char *>(&header), sizeof(Header));

  // Vérification de la validité du fichier
  if (header.ident != IDENT || header.version != VERSION)
  {
    std::cerr << "Mauvais type de fichier\n";
    exit(-1);
  }

  // Allocation mémoire
  skins.resize(header.num_skins);
  texCoords.resize(header.num_st);
  triangles.resize(header.num_tris);
  frames.resize(header.num_frames);

  // Lecture des noms des skins
  ifs.seekg(header.offset_skins, std::ios::beg);
  ifs.read(reinterpret_cast<char *>(skins.data()), sizeof(Skin) *header.num_skins);

  // Lecture des coordonnées de texture
  ifs.seekg(header.offset_st, std::ios::beg);
  ifs.read(reinterpret_cast<char *>(texCoords.data()), sizeof(TexCoord) * header.num_st);

  // Lecture de la connectivité
  ifs.seekg(header.offset_tris, std::ios::beg);
  ifs.read(reinterpret_cast<char *>(triangles.data()), sizeof(Triangle) * header.num_tris);

  // Lecture des positions pour chaque animation
  ifs.seekg(header.offset_frames, std::ios::beg);
  std::vector<CompressedVertex> compressed_verts;
  for (int i = 0; i < header.num_frames; i++)
  {
    ifs.read(reinterpret_cast<char *>(&frames[i].scale), sizeof(vec3));
    ifs.read(reinterpret_cast<char *>(&frames[i].translate), sizeof(vec3));
    ifs.read(reinterpret_cast<char *>(&frames[i].name), 16);
    compressed_verts.resize(header.num_vertices);
    ifs.read(reinterpret_cast<char *>(compressed_verts.data()), sizeof(CompressedVertex) * header.num_vertices);
    for (int k = 0; k < header.num_vertices; k++)
      frames[i].verts.push_back(vec3(compressed_verts[k].v[0],
                                     compressed_verts[k].v[1],
                                     compressed_verts[k].v[2]));
  }

  ifs.close();

  // Mise en place des animations
  setup_animations();
}

/***************************************************************************\
 * Md2::Model::load_texture                                                *
 * Charge une texture depuis un fichier et l'ajoute à la liste des skins   *
\***************************************************************************/
bool Md2::Model::load_texture(const std::string &filename)
{
  GLuint tex = texture_manager.get_texture(filename);

  skin_ids.insert(SkinMap::value_type(filename, tex));

  return true;
}

/***************************************************************************\
 * Md2::Model::set_texture                                                 *
 * Choisitun texture comme skin courante                                   *
\***************************************************************************/
void Md2::Model::set_texture(const std::string &filename)
{
  auto iter = skin_ids.find(filename);

  if (iter != skin_ids.end())
    tex = iter->second;
  else
    tex = 0;
}

/***************************************************************************\
 * Md2::Model::setup_animations                                            *
 * Crée la liste des animations.                                           *
\***************************************************************************/
void Md2::Model::setup_animations()
{
  std::string current_anim;
  Anim anim_info = { 0, 0 };

  for (int i = 0; i < header.num_frames; i++)
  {
    std::string frame_name = frames[i].name;
    std::string frame_anim;

    // Extraction des noms d'animations à partir des noms des positions
    std::string::size_type len = frame_name.find_first_of("0123456789");
    if ((len == frame_name.length () - 3) &&
        (frame_name[len] != '0'))
      len++;

    frame_anim.assign(frame_name, 0, len);

    if (current_anim != frame_anim)
    {
      if (i > 0)
        anims.insert(AnimMap::value_type(current_anim, anim_info));
      // Passage à l'animation suivante
      anim_info.start = i;
      anim_info.end = i;

      current_anim = frame_anim;
    }
    else
      anim_info.end = i;
  }

  // Ajout de la dernière animation
  anims.insert(AnimMap::value_type(current_anim, anim_info));
}

// Position de la lumière
vec3 light_pos{ 0, -2, 10 };

/***************************************************************************\
 * Md2::Model::draw_model                                                  *
\***************************************************************************/
void Md2::Model::draw_model(int frameA, int frameB, float interp)
{
  // vecteurs pour stocker les positions et les coordonnées de texture du personnage
  std::vector<vec3> positions;
  std::vector<vec2> tex_coords;

  // vecteur pour stocker les positions de l'ombre.
  std::vector<vec3> positions_ombres;

  // *** matrice de projection permettant de calculer une ombre projetée pour la technique 
  // *** de la projection centrale
  // *** matrix proj;

  // Calcul de chaque triangle
  for (int i = 0; i < header.num_tris; ++i)
  {
    // Calcul pour chaque sommet de ce triangle
    for (int j = 0; j < 3; ++j)
    {
      Frame *pFrameA = &frames[frameA];
      Frame *pFrameB = &frames[frameB];

      vec3 *pVertA = &pFrameA->verts[triangles[i].vertex[j]];
      vec3 *pVertB = &pFrameB->verts[triangles[i].vertex[j]];

      // Décompression des positions
      vec3 vecA = pFrameA->scale * *pVertA + pFrameA->translate;
      vec3 vecB = pFrameB->scale * *pVertB + pFrameB->translate;

      // Interpolation linéaire et mise à l'echelle
      vec3 v = (vecA + interp * (vecB - vecA)) * scale;
      positions.push_back(v); // Ajout d'une position dans le tableau
      
      // *** C'est ici qu'il faut ajouter le code pour calculer les positions de l'ombre

//      vec3 v_ombre = v;
//      v_ombre.z = -2.4;
//      positions_ombres.push_back( v_ombre ); Q1, 2 et 3
      //float z= -2.4;
      vec4 P(0,0,1,2.41);
      vec4 light(light_pos.x,light_pos.y,light_pos.z,0);
      vec4 v0(P.dot(light) - light.x * P.x, -light.x * P.y, -light.x * P.z, -P.w * light.x );
      vec4 v1(-light.y * P.x, P.dot(light) - light.y * P.y, -light.y * P.z, -P.w * light.y );
      vec4 v2(-light.z * P.x, -light.z * P.x, P.dot(light) - light.z * P.z, -P.w * light.z );
      vec4 v3(-light.w * P.x, -light.w * P.y, -light.w * P.y, P.dot(light) - P.w * light.w );

      matrix M(v0,v1,v2,v3);
      positions_ombres.push_back(M*v);

      //
      TexCoord *pTexCoords = &texCoords[triangles[i].st[j]];
      // Calcul des coordonnées de textures
      float s = static_cast<float>(pTexCoords->s) / header.skinwidth;
      float t = static_cast<float>(pTexCoords->t) / header.skinheight;
      tex_coords.push_back(vec2(s, 1 - t));
    }
  }
  glDisable(GL_BLEND);
  glDepthFunc(GL_LESS);

  // Dessin de l'ombre si elle existe
  if (!positions_ombres.empty())
  {
    glColor4f(0.2,0.2,0.2,1);
    glDisable(GL_TEXTURE_2D);
    glVertexPointer(3, GL_FLOAT, 0, positions_ombres.data());
    glDrawArrays(GL_TRIANGLES, 0, positions_ombres.size());
  }

  // Dessin du personnage
  glColor4f(1,1,1,1);
  glTexCoordPointer(2, GL_FLOAT, 0, tex_coords.data());
  glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glVertexPointer(3, GL_FLOAT, 0, positions.data());
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, tex);
  glEnable(GL_TEXTURE_2D);
  glDrawArrays(GL_TRIANGLES, 0, positions.size());
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/***************************************************************************\
 * Md2::Object::Object                                                     *
\***************************************************************************/
Md2::Object::Object () : model(nullptr), current_frame(0), next_frame(0),
    interp(0.0f), percent(0.0f), scale(1)
{
}

/***************************************************************************\
 * Md2::Object::draw_object_itp                                            *
\***************************************************************************/
void Md2::Object::draw_object_itp(bool animated)
{
  glPushMatrix ();
    glRotatef(-90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);

    model->set_scale(scale);

    glPushAttrib (GL_POLYGON_BIT);
    glFrontFace (GL_CW);

    // Dessin du personnage et de son ombre
    model->draw_model(current_frame, next_frame, interp);

    glPopAttrib ();
  glPopMatrix ();

  if (animated)
    interp += percent;
}

/***************************************************************************\
 * Md2::Object::animate                                                    *
 * Animation du personnage. Calcule la position courante, la suivante et   *
 * le pourcentage de l'interpolation.                                      *
\***************************************************************************/
void Md2::Object::animate(int start_frame, int end_frame, float p)
{
  if (current_frame < start_frame)
    current_frame = start_frame;

  if (current_frame > end_frame)
    current_frame = start_frame;

  percent = p;

  if (interp >= 1.0)
    {
      interp = 0.0f;
      current_frame++;

      if (current_frame >= end_frame)
        current_frame = start_frame;

      next_frame = current_frame + 1;

      if (next_frame >= end_frame)
        next_frame = start_frame;
    }
}

/***************************************************************************\
 * Md2::Object::animate                                                    *
\***************************************************************************/
void Md2::Object::animate(float p)
{
  // Use the current animation
  animate(anim_info->start, anim_info->end, p);
}

/***************************************************************************\
 * Md2::Object::set_model                                                  *
\***************************************************************************/
void Md2::Object::set_model(Model *m)
{
  model = m;

  if (model)
  {
    anim_info = &model->get_anims().begin()->second;
    current_anim = model->get_anims().begin()->first;
  }
}

/***************************************************************************\
 * Md2::Object::set_anim                                                   *
\***************************************************************************/
void Md2::Object::set_anim(const std::string &name)
{
  Model::AnimMap::const_iterator itor;
  itor = model->get_anims().find(name);

  if (itor != model->get_anims().end())
  {
    anim_info = &itor->second;
    current_anim = name;
  }
}
