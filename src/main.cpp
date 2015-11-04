// main.cpp

#include <iostream>
#include <cstring>
#include <GL/glut.h>

#include "md2_player.h"

struct mouse_input_t
{
  int buttons[3];
  int x, y;

} mouse;

// Timer
struct glut_timer_t
{
  double current_time;
  double last_time;

} timer;

int modifiers;

// Camera
vec3 rot(0, 0, 0), eye(0, 0, 8);

Md2::Player *player = nullptr;

bool animated = true;

int frame_rate = 7;

std::vector<std::string> all_skins;
std::vector<std::string> all_anims;
extern vec3 light_pos;

/*=========================================================================*\
 * anim_menu_callback                                                      *
 *                                                                         *
 * GLUT menu callback function. Handle the anim menu.                      *
 * Select the animation to play.                                           *
\*=========================================================================*/
static void anim_menu_callback(int item)
{
  player->set_anim(all_anims[item]);

  glutPostRedisplay();
}

/*=========================================================================*\
 * skin_menu_callback                                                      *
 *                                                                         *
 * GLUT menu callback function. Handle the skin menu.                      *
 * Select the skin to use.                                                 *
\*=========================================================================*/
static void skin_menu_callback(int item)
{
  player->set_skin(all_skins[item]);

  glutPostRedisplay();
}

/*=========================================================================*\
 * build_skin_menu                                                         *
 *                                                                         *
 * Create GLUT menu for skin selection.                                    *
\*=========================================================================*/
static int build_skin_menu(const Md2::Model::SkinMap &skin_map)
{
  int menu_id = glutCreateMenu(skin_menu_callback);
  int i = 0;

  for (auto &skin : skin_map)
  {
    std::string skin_name = skin.first;
    all_skins.push_back(skin.first);
    skin_name.assign(skin_name, skin_name.find_last_of('/') + 1, skin_name.length());
    glutAddMenuEntry(skin_name.c_str(), i++);
  }

  return menu_id;
}

/*=========================================================================*\
 * build_anim_menu                                                         *
 *                                                                         *
 * Create GLUT menu for animation selection.                               *
\*=========================================================================*/
static int build_anim_menu(const Md2::Model::AnimMap &anim_map)
{
  int menu_id = glutCreateMenu(anim_menu_callback);
  int i = 0;

  for (auto &anim : anim_map)
  {
    all_anims.push_back(anim.first);
    glutAddMenuEntry(anim.first.c_str(), i++);
  }

  return menu_id;
}

/*=========================================================================*\
 * shutdown_app                                                            *
 *                                                                         *
 * Application termination, cleaning up                                    *
\*=========================================================================*/
static void shutdown_app()
{
  delete player;
}

/*=========================================================================*\
 * init                                                                    *
 *                                                                         *
 * Application initialization.  Setup keyboard input, mouse input,         *
 * timer, camera and OpenGL.                                               *
\*=========================================================================*/
static void init(const std::string &path)
{
  // Inititialize mouse
  mouse.buttons[GLUT_LEFT_BUTTON] = GLUT_UP;
  mouse.buttons[GLUT_MIDDLE_BUTTON] = GLUT_UP;
  mouse.buttons[GLUT_RIGHT_BUTTON] = GLUT_UP;
  mouse.x = 0;
  mouse.y = 0;

  // Initialize timer
  timer.current_time = 0;
  timer.last_time = 0;

  // Remove the trailing slash
  std::string dirname (path);
  if (dirname.find_last_of ('/') == dirname.length () - 1)
    dirname.assign (dirname, 0, dirname.find_last_of ('/'));

  // Get base dir for player if a *.md2 file is given
  if (dirname.find (".md2") == dirname.length () - 4)
    dirname.assign (dirname, 0, dirname.find_last_of ('/'));

  // Load MD2 models
  try
  {
    player = new Md2::Player (dirname);

    player->set_scale(0.1f);
  }
  catch (std::runtime_error &err)
  {
    std::cerr << "Error: failed to load player from " << dirname << std::endl;
    std::cerr << "Reason: " << err.what () << std::endl;

    if (errno)
      std::cerr << strerror(errno) << std::endl;

    exit (-1);
  }

  // Create GLUT menus
  const Md2::Model *ref = player->get_player_mesh();

  int skinMenuId = build_skin_menu(ref->get_skins());
  int animMenuId = build_anim_menu(ref->get_anims());

  glutCreateMenu(nullptr);
  glutAddSubMenu("Skin", skinMenuId);
  glutAddSubMenu("Animation", animMenuId);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  player->set_anim("stand");

  // Initialize OpenGL
  glClearColor(0.5, 0.5, 0.5, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnableClientState(GL_VERTEX_ARRAY);
}


/*=========================================================================*\
 * reshape_callback                                                        *
 * OpenGL window resizing.  Update the viewport and the projection matrix. *
\*=========================================================================*/
static void reshape_callback(int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, static_cast<float>(w) / h, 0.1, 1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glutPostRedisplay();
}

/*=========================================================================*\
 * Update the timer.                                                       *
\*=========================================================================*/
static void update_timer(struct glut_timer_t *t)
{
  t->last_time = t->current_time;
  t->current_time = glutGet (GLUT_ELAPSED_TIME) * 0.001f;
}

/*=========================================================================*\
 * display_callback                                                        *
\*=========================================================================*/
static void display_callback()
{
  // Animation
  if (animated)
  {
    double dt = timer.current_time - timer.last_time;
    player->animate(frame_rate * dt);
  }

  // Clear window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glLoadIdentity();

  // Perform camera transformations
  glTranslatef(-eye.x, -eye.y, -eye.z);
  glRotatef(rot.x, 1, 0, 0);
  glRotatef(rot.y, 0, 1, 0);
  glRotatef(rot.z, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_TEXTURE_2D);

  // Draw objects
  player->draw_player_itp(animated);

  glutSwapBuffers();
}

/*=========================================================================*\
 * keypress_callback                                                       *
 * Key press glut callback function.  Called when user press a key.        *
\*=========================================================================*/
static void keypress_callback(unsigned char key, int, int)
{
  switch (key)
  {
    case 27: exit(0);
    case 'a': case 'A':
             animated = !animated;
             break;
    case 's': case 'S':
             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
             break;
    case 'w': case 'W':
             glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
             break;
    case '+': frame_rate++; break;
    case '-': frame_rate--; break;
  }
  if (frame_rate < 0)
    frame_rate = 0;

  glutPostRedisplay();
}

/*=========================================================================*\
 * Key press glut callback function.  Called when user press a special key.*
\*=========================================================================*/
static void special_key_press_callback(int key, int, int)
{
  switch (key)
  {
    case GLUT_KEY_UP:
      light_pos.x += 0.1;
      break;
    case GLUT_KEY_DOWN:
      light_pos.x -= 0.1;
      break;
    case GLUT_KEY_RIGHT:
      light_pos.y += 0.1;
      break;
    case GLUT_KEY_LEFT:
      light_pos.y -= 0.1;
      break;
  }
}

/*=========================================================================*\
 * mouse_motion_callback                                                   *
 * Mouse motion glut callback function.  Called when the user move the     *
 * mouse. Update the camera.                                               *
\*=========================================================================*/
static void mouse_motion_callback(int x, int y)
{
  if (mouse.buttons[GLUT_MIDDLE_BUTTON] == GLUT_DOWN)
    eye.z += (x - mouse.x) * 0.1; // Zoom
  else if (mouse.buttons[GLUT_LEFT_BUTTON] == GLUT_DOWN)
  {
    if (modifiers & GLUT_ACTIVE_SHIFT)
    { // Translation
      eye.x -= (x - mouse.x) * 0.02;
      eye.y += (y - mouse.y) * 0.02;
    }
    else
    { // Rotation
      rot.x += (y - mouse.y);
      rot.y += (x - mouse.x);
    }
  }

  mouse.x = x;
  mouse.y = y;

  glutPostRedisplay ();
}

/*=========================================================================*\
 * mouse_button_callback                                                   *
 * Mouse button press glut callback function.  Called when the user        *
 * press a mouse button. Update mouse state and keyboard modifiers.        *
\*=========================================================================*/
static void mouse_button_callback(int button, int state, int x, int y)
{
  // Update key modifiers
  modifiers = glutGetModifiers();

  // Update mouse state
  mouse.buttons[button] = state;
  mouse.x = x;
  mouse.y = y;
}

/*=========================================================================*\
 * idle_callback                                                           *
 * Idle glut callback function. Continuously called. Perform background    *
 * processing tasks.                                                       *
\*=========================================================================*/
static void idle_callback()
{
  // Update the timer
  update_timer(&timer);

  if (animated)
    glutPostRedisplay();
}

/*=========================================================================*\
 * window_status_callback                                                  *
 * Window status glut callback function.  Called when the status of        *
 * the window changes.                                                     *
\*=========================================================================*/
static void window_status_callback(int state)
{
  // Disable rendering and animation when the window is not visible
  if (state != GLUT_HIDDEN && state != GLUT_FULLY_COVERED)
    glutIdleFunc(idle_callback);
  else
    glutIdleFunc(nullptr);
}

int main(int argc, char *argv[])
{
  std::string path = "./data/";
  // Initialize GLUT
  glutInit (&argc, argv);

  if (argc >= 2) path = argv[1];

  // create an OpenGL window
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH |  GLUT_STENCIL);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Ombres, z-zero");

  // Initialize application
  atexit(shutdown_app);
  init(path);

  // Setup glut callback functions
  glutReshapeFunc(reshape_callback);
  glutDisplayFunc(display_callback);
  glutKeyboardFunc(keypress_callback);
  glutSpecialFunc(special_key_press_callback);
  glutMotionFunc(mouse_motion_callback);
  glutMouseFunc(mouse_button_callback);
  glutWindowStatusFunc(window_status_callback);
  glutIdleFunc(idle_callback);

  // Enter the main loop
  glutMainLoop();
}
