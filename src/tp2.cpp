
#include "texture.h"
#include "wavefront.h"

#include "student/all.hpp"
#include "app.h"
#include "draw.h"
#include "orbiter.h"

#include <iostream>
#include <memory>
#include <vector>

using namespace stu;

struct curve {
  std::vector<Transform> nodes = {};
};

auto circle(std::size_t n) {
    auto v = std::vector<Point>(n);
    for(std::size_t i = 0; i < n; ++i) {
        v[i] = RotationX(360.f / n * i)(Point(0, 1, 0));
    }
    return v;
}

auto tube(const curve& c, int n = 10) {
    auto m = std::make_unique<Mesh>(GL_TRIANGLES);
    auto ci = circle(n);
    for(auto& p : ci) p = p / 10.f;
    for(std::size_t i = 1; i < c.nodes.size(); ++i) {
        auto& n0 = c.nodes[i - 1];
        auto& n1 = c.nodes[i];
        for(std::size_t i = 0; i < n; ++ i) {
            m->vertex(n0(ci[i]));
            m->vertex(n0(ci[(i + 1) % n]));
            m->vertex(n1(ci[(i + 1) % n]));

            m->vertex(n0(ci[i]));
            m->vertex(n1(ci[(i + 1) % n]));  
            m->vertex(n1(ci[i]));
        }
    }
    return m;
}

auto debug_mesh(const curve &c) {
  auto m = std::make_unique<Mesh>(GL_LINES);
  for (auto &&n : c.nodes) {
      std::cout << w(n) << std::endl;
    m->color(Red());
    m->vertex(w(n));
    m->vertex(w(n) + x(n) / 5);
    m->color(Green());
    m->vertex(w(n));
    m->vertex(w(n) + y(n) / 5);
    m->color(Blue());
    m->vertex(w(n));
    m->vertex(w(n) + z(n) / 5);
  }
  return m;
}

auto mesh(const curve &c) {
  auto m = std::make_unique<Mesh>(GL_LINES);
  for (std::size_t i = 1; i < c.nodes.size(); ++i) {
    auto &n0 = c.nodes[i - 1];
    auto &n1 = c.nodes[i];
    m->vertex(w(n0));
    m->vertex(w(n1));
  }
  return m;
}

void subdivide_chaikin(curve &c) {
  auto ns = std::vector<Transform>();
  ns.push_back(c.nodes.front());
  for (std::size_t i = 1; i < c.nodes.size(); ++i) {
    auto& n0 = c.nodes[i - 1];
    auto& n1 = c.nodes[i];
    ns.push_back(n0 + (n1 - n0) / 4.f);
    ns.push_back(n0 + (n1 - n0) * 3.f / 4.f);
  }
  ns.push_back(c.nodes.back());
  c.nodes = std::move(ns);
}

void compute_forward(curve &c) {
    if (c.nodes.size() < 2) throw std::logic_error("");
    x(c.nodes[0], normalize(w(c.nodes[1]) - w(c.nodes[0])));
    for (std::size_t i = 2; i < c.nodes.size(); ++i) {
        x(c.nodes[i - 1], normalize(w(c.nodes[i]) - w(c.nodes[i - 2])));
    }
    x(c.nodes[c.nodes.size() - 1], normalize(w(c.nodes[c.nodes.size() - 1]) - w(c.nodes[c.nodes.size() - 2])));
}

void compute_z(curve &c) {
    z(c.nodes.front(), normalize(perpendicular(x(c.nodes.front()))));

    for(std::size_t i = 1; i < c.nodes.size(); ++i) {
        auto &n0 = c.nodes[i - 1];
        auto &n1 = c.nodes[i];

        auto r = rotation(x(n0), x(n1));
        z(n1, normalize(r(z(n0))));
    }
}

void compute_y(curve &c) {
    for(auto&& n : c.nodes) {
        y(n, cross(z(n), x(n)));
    }
}


class TP : public App {
public:
    TP() : App(1024, 640) {}

    int init() {
        m_camera.lookat(Point(0, 0, 0), 5);

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClearDepth(1.f);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        auto c = curve();
        /*c.nodes.push_back(Translation(0, 0, 0));
        c.nodes.push_back(Translation(0, 0, 1));
        c.nodes.push_back(Translation(0, 1, 1));
        c.nodes.push_back(Translation(0, 1, 0));
        c.nodes.push_back(Translation(1, 1, 0));
        c.nodes.push_back(Translation(1, 1, 1));
        c.nodes.push_back(Translation(1, 0, 1));
        c.nodes.push_back(Translation(1, 0, 0));*/

        c.nodes.push_back(Translation(0, 0, 0));
        c.nodes.push_back(Translation(1, 0, 0));
        c.nodes.push_back(Translation(1, 1, 0));
        c.nodes.push_back(Translation(0, 1, 0));
        c.nodes.push_back(Translation(0, 0, 0));

        subdivide_chaikin(c);
        subdivide_chaikin(c);
        subdivide_chaikin(c);
        subdivide_chaikin(c);

        compute_forward(c);
        compute_z(c);
        compute_y(c);

        curve_mesh = tube(c, 30);
        debug_mesh = ::debug_mesh(c);

        return 0;
    }

    int quit() {
        curve_mesh->release();
        return 0;
    }

    int render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // deplace la camera
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        if (mb & SDL_BUTTON(1)) // le bouton gauche est enfonce
            m_camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3)) // le bouton droit est enfonce
            m_camera.move(mx);
        else if (mb & SDL_BUTTON(2)) // le bouton du milieu est enfonce
            m_camera.translation((float)mx / (float)window_width(),
                                (float)my / (float)window_height());

        draw(*debug_mesh, m_camera, 0);
        // draw(*curve_mesh, m_camera, 0);

        return 1;
    }

protected:
    Orbiter m_camera;

    std::unique_ptr<Mesh> curve_mesh;
    std::unique_ptr<Mesh> debug_mesh;
};

void throwing_main() {
    auto a = Vector(0, 0, 1);
    auto b = Vector(0, 1, 0);
    rotation(a, b);
    TP tp;
    tp.run();
}

int main(int, char**) {
    try {
        throwing_main();
    } catch(const std::exception &e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return -1;
    } catch(...) {
        std::cerr << "Unhandled exception." << std::endl;
        return -1;
    }
    return 0;
}
