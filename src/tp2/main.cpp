
#include "wavefront.h"
 #include "texture.h"
  
 #include "orbiter.h"
 #include "draw.h"        
 #include "app.h"

#include <vector>

struct node {
    Point xyz = {};
};

struct curve {
    std::vector<node> nodes = {};
};

Mesh mesh(const curve& c) {
    auto m = Mesh(GL_LINES);
    for(std::size_t i = 1; i < nodes.size(); ++i) {
        auto& n0 = c.nodes[i - 1];
        auto& n1 = c.nodes[i];
        m.vertex(n0);
        m.vertex(n1);
    }
    return m;
}

void subdivide_chaikin(curve& c) {
    auto ns = std::vector<node>();
    for(std::size_t i = 1; i < nodes.size(); ++i) {
        auto& n0 = c.nodes[i - 1];
        auto& n1 = c.nodes[i];
        ns.push_back({ .xyz = n0 + (n1 - n0) / 3.f });
        ns.push_back({ .xyz = n0 + (n1 - n0) * 2.f / 3.f });
    }
    c.nodes = std::move(ns);
}

 class TP : public App
 {
 public:
     // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
     TP( ) : App(1024, 640) {}
     
     // creation des objets de l'application
     int init( ) {
         m_camera.lookat(pmin, pmax);
         
         // etat openGL par defaut
         glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
         
         glClearDepth(1.f);                          // profondeur par defaut
         glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
         glEnable(GL_DEPTH_TEST);                    // activer le ztest
  
         auto c = curve();
         c.nodes.push_back({ .xyz = {1, 0, 0} });
         c.nodes.push_back({ .xyz = {0, 1, 0} });
         c.nodes.push_back({ .xyz = {0, 0, 1} });

         subdivide_chaikin(c);

         // curve_mesh = mesh(c);

         return 0;   // ras, pas d'erreur
     }
     
     // destruction des objets de l'application
     int quit() {
         glDeleteTextures(1, &m_texture);
         
         return 0;
     }
     
     // dessiner une nouvelle image
     int render( )
     {
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
         // deplace la camera
         int mx, my;
         unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
         if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
             m_camera.rotation(mx, my);
         else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
             m_camera.move(mx);
         else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
             m_camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());
         
         draw(curve_mesh, m_camera, 0);
         
         return 1;
     }
  
 protected:
     Orbiter m_camera;

     Mesh curve_mesh;
 };
  
  
 int main( int argc, char **argv )
 {
     // il ne reste plus qu'a creer un objet application et la lancer 
     TP tp;
     tp.run();
     
     return 0;
 }
