
#include "texture.h"
#include "wavefront.h"

#include "student/all.hpp"
#include "app.h"
#include "draw.h"
#include "program.h"
#include "orbiter.h"
#include "uniforms.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

using namespace stu;

struct curve {
    std::vector<Transform> nodes = {};
};

Transform at(const curve& c, float t) {
    auto s = float(c.nodes.size());
    if(t < 0.f) {
        return c.nodes.front();
    } else {
        t = t - std::floor(t / (s - 1)) * (s - 1);
        float i;
        auto f = std::modf(t, &i);
        return (1.f - f) * c.nodes[i] + f * c.nodes[i + 1];
    }
}

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
    if(i != 1)                  ns.push_back(n0 + (n1 - n0) / 4.f);
    if(i != c.nodes.size() - 1) ns.push_back(n0 + (n1 - n0) * 3.f / 4.f);
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
        m_camera.lookat(Point(0, 0, 0), 2);

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClearDepth(1.f);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        c = curve();

        c.nodes.push_back(Translation(25, 0, 0));
        c.nodes.push_back(Translation(50, 0, 0));
        c.nodes.push_back(Translation(50, 50, 0));
        c.nodes.push_back(Translation(0, 50, 0));
        c.nodes.push_back(Translation(0, 0, 0));
        c.nodes.push_back(Translation(25, 0, 0));

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

    int update(float t, float dt) override {
        int n;
        if(key_state(SDLK_LEFT)) {
            std::cout << "ok" << std::endl;
            angle -= .1f;
        } else if(key_state(SDLK_RIGHT)) {
            angle += .1f;
        }

        time = t / 100.f;
        auto ct = at(c, time);

        auto r = RotationX(deg_per_rad * angle);

        camera = Lookat(w(ct) - 5.f * x(ct) + 3.f * r(y(ct)), w(ct), r(y(ct)));

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

        Transform v = camera;
        Transform vp = Perspective(90.f, 16.f / 9.f, 1.f, 1000.f) * v;
    
        glUseProgram(mesh_program);
        program_uniform(mesh_program, "mesh_color", Color(1, 1, 1, 1));
        program_uniform(mesh_program, "mvMatrix", v);
        program_uniform(mesh_program, "mvpMatrix", vp);
        curve_mesh->draw(mesh_program, true, false, false, false, false);

        glUseProgram(mesh_color_program);
        program_uniform(mesh_color_program, "mvpMatrix", vp);
        debug_mesh->draw(mesh_color_program, true, false, false, true, false);

        return 1;
    }

protected:
    GLuint mesh_program = read_program(
        smart_path("data/shaders/mesh.glsl"));
    GLuint mesh_color_program = read_program(
        smart_path("data/shaders/mesh_color.glsl"),
        "#define USE_COLOR\n");

    Transform camera;

    Orbiter m_camera;

    float angle = 0.f;
    float time = 0.f;

    curve c;

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
