#include "box.hpp"

#include "window.h"
#include "wavefront.h"
#include "draw.h"

Mesh green ;
Mesh red ;

Box b1 ;
Box b2 ;

Orbiter camera ;

int init()
{
  green = read_mesh(smart_path("data/cube.obj")) ;
  green.default_color(Green()) ;

  red = read_mesh(smart_path("data/cube.obj")) ;
  red.default_color(Red()) ;

  Point pmin, pmax ;
  green.bounds(pmin, pmax) ;
  b1 = Box(pmin, pmax) ;
  b2 = Box(pmin, pmax) ;

  camera.lookat(Point(-2., -2., -2.), Point(2., 2., 2.));

  // etat openGL par defaut
  glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

  // etape 3 : configuration du pipeline.
  glClearDepth(1.f);                          // profondeur par defaut
  glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
  glEnable(GL_DEPTH_TEST);                    // activer le ztest

  return 0 ;
}

int draw( )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // recupere les mouvements de la souris, utilise directement SDL2
  int mx, my;
  unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

  // deplace la camera
  if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
      // tourne autour de l'objet
      camera.rotation(mx, my);

  else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
      // approche / eloigne l'objet
      camera.move(mx);

  else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
      // deplace le point de rotation
      camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

  unsigned int elapsed = SDL_GetTicks() ;

  Vector t(0.65, 0, 0) ;
  t = RotationZ(-0.005 * elapsed)(t) ;

  b1.T = Translation(t) ;
  b2.T = Translation(-t) ;

  /*Decommenter dans un second temps*/
  b1.T = b1.T * RotationZ(-0.003 * elapsed) * RotationY(0.05 * elapsed);
  b2.T = b2.T * RotationZ(-0.002 * elapsed) * RotationY(0.05 * elapsed);

  if(b1.collides(b2)) {
    draw(green, b1.T, camera) ;
    draw(green, b2.T, camera) ;
  } else {
    draw(red, b1.T, camera) ;
    draw(red, b2.T, camera) ;
  }

  return 1;
}

int quit( )
{
    // etape 3 : detruire la description du triangle
    return 0;   // ras, pas d'erreur
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        // erreur lors de la creation de la fenetre ou de l'init de sdl2
        return 1;       

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)
        // erreur lors de la creation du contexte opengl
        return 1;       

    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application
    run(window, draw);

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}
