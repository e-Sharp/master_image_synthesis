
#include "texture.h"
#include "wavefront.h"

#include "src/student/all.hpp"
#include "app.h"
#include "draw.h"
#include "program.h"
#include "orbiter.h"
#include "uniforms.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <ctime>

using namespace stu;

GLuint read_cubemap( const int unit, const char *filename,  const GLenum texel_type = GL_RGBA )
{
    // les 6 faces sur une croix
    ImageData image= read_image_data(filename);
    if(image.pixels.empty())
        return 0;

    int w= image.width / 4;
    int h= image.height / 3;
    assert(w == h);

    GLenum data_format;
    GLenum data_type= GL_UNSIGNED_BYTE;
    if(image.channels == 3)
        data_format= GL_RGB;
    else // par defaut
        data_format= GL_RGBA;

    // creer la texture
    GLuint texture= 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // creer les 6 faces
    // chaque face de la cubemap est un rectangle [image.width/4 x image.height/3] dans l'image originale
    struct { int x, y; } faces[]= {
        {0, 1}, // X+
        {2, 1}, // X-
        {1, 2}, // Y+
        {1, 0}, // Y-
        {1, 1}, // Z+
        {3, 1}, // Z-
    };

    for(int i= 0; i < 6; i++)
    {
        ImageData face= flipX(flipY(copy(image, faces[i].x*w, faces[i].y*h, w, h)));

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
            texel_type, w, h, 0,
            data_format, data_type, face.data());
    }

    // parametres de filtrage
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // filtrage "correct" sur les bords du cube...
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //~ glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    printf("  cubemap faces %dx%d\n", w, h);

    return texture;
}

Transform at(const Tube& c, float t) {
    auto s = float(c.nodes.size());
    if(t < 0.f) {
        return c.nodes.front();
    } else {
        t = mod(t, float(s - 1));
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

auto tube(const Tube& c, int n = 10) {
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

auto debug_mesh(const Tube &c) {
  auto m = std::make_unique<Mesh>(GL_LINES);
  for (auto &&n : c.nodes) {
    m->color(Red());
    m->vertex(pos(n));
    m->vertex(pos(n) + fw(n) / 5);
    m->color(Green());
    m->vertex(pos(n));
    m->vertex(pos(n) + up(n) / 5);
    m->color(Blue());
    m->vertex(pos(n));
    m->vertex(pos(n) + right(n) / 5);
  }
  return m;
}

auto mesh(const Tube &c) {
  auto m = std::make_unique<Mesh>(GL_LINES);
  for (std::size_t i = 1; i < c.nodes.size(); ++i) {
    auto &n0 = c.nodes[i - 1];
    auto &n1 = c.nodes[i];
    m->vertex(pos(n0));
    m->vertex(pos(n1));
  }
  return m;
}

void subdivide_chaikin(Tube &c) {
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

void compute_forward(Tube &c) {
    if (c.nodes.size() < 2) throw std::logic_error("");
    fw(c.nodes[0], normalize(pos(c.nodes[1]) - pos(c.nodes[0])));
    for (std::size_t i = 2; i < c.nodes.size(); ++i) {
        fw(c.nodes[i - 1], normalize(pos(c.nodes[i]) - pos(c.nodes[i - 2])));
    }
    fw(c.nodes[c.nodes.size() - 1], normalize(pos(c.nodes[c.nodes.size() - 1]) - pos(c.nodes[c.nodes.size() - 2])));
}

void compute_z(Tube &c) {
    right(c.nodes.front(), normalize(perpendicular(fw(c.nodes.front()))));

    for(std::size_t i = 1; i < c.nodes.size(); ++i) {
        auto &n0 = c.nodes[i - 1];
        auto &n1 = c.nodes[i];

        auto r = rotation(fw(n0), fw(n1));
        right(n1, normalize(r(right(n0))));
    }
}

void compute_y(Tube &c) {
    for(auto&& n : c.nodes) {
        up(n, cross(right(n), fw(n)));
    }
}

class TP : public App {
public:
    TP() : App(1024, 640) {}

    int init() {
        program_uniform(m_program_draw, "ww", 1024);
        program_uniform(m_program_draw, "wh", 640);

        m_camera.lookat(Point(0, 0, 0), 2);

        glGenVertexArrays(1, &m_vao);

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClearDepth(1.f);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        c = Tube();

        std::srand(std::time(nullptr));

        for (size_t i=0; i<NB_POINTS; ++i) {
            ra += std::rand()%(STEP);
            rb += std::rand()%(STEP);
            rc += std::rand()%(STEP);
            c.nodes.push_back(Translation(ra, rb, rc));
        }

        subdivide_chaikin(c);
        subdivide_chaikin(c);
        subdivide_chaikin(c);
        subdivide_chaikin(c);

        compute_forward(c);
        compute_z(c);
        compute_y(c);

        curve_mesh = tube(c, 30);
        debug_mesh = ::debug_mesh(c);

        obstacles.resize(100);
        for(int i = 0; i < 100; ++i) {
            auto& o = obstacles[i];
            o.azimuth = float(i);
            o.coordinate = float(i);
            o.radius = 2.f;
        }

        player.coords.radius = 2.f;

        return 0;
    }

    int quit() {
        curve_mesh->release();
        return 0;
    }

    int update(float t, float dt) override {
        if(key_state(SDLK_LEFT)) {
            player.turn_left();
        } else if(key_state(SDLK_RIGHT)) {
            player.turn_right();
        }

        player.update();

        auto ct = at(c, player.coords.coordinate);

        player_transform = ct * RotationX(deg_per_rad * player.coords.azimuth) * Translation(0, player.coords.radius, 0);

        camera = Lookat(pos(player_transform) - 5.f * fw(player_transform) + 3.f * up(player_transform), pos(player_transform), up(player_transform));

        {
            player_collider.T = player_transform;
        }

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

        for(const auto& o : obstacles) {
            auto model = at(c, o.coordinate) * RotationX(o.azimuth) * Translation(0.f, o.radius, 0.f);
            auto collider = Box();
            collider.T = model;

            glUseProgram(mesh_program);
            if(collides(collider, player_collider)) {
                program_uniform(mesh_program, "mesh_color", Color(1, 0, 0, 1));
            } else {
                program_uniform(mesh_program, "mesh_color", Color(0, 1, 0, 1));
            }
            program_uniform(mesh_program, "mvMatrix", v * model);
            program_uniform(mesh_program, "mvpMatrix", vp * model);
            obstable_mesh.draw(mesh_program, true, false, false, false, false);
        }

        {
            glUseProgram(mesh_program);
            program_uniform(mesh_program, "mesh_color", Color(1, 1, 1, 1));
            program_uniform(mesh_program, "mvMatrix", v * player_transform);
            program_uniform(mesh_program, "mvpMatrix", vp * player_transform);
            player.mesh.draw(mesh_program, true, false, false, false, false);
        }

        glUseProgram(mesh_color_program);
        program_uniform(mesh_color_program, "mvpMatrix", vp);
        debug_mesh->draw(mesh_color_program, true, false, false, true, false);

        glUseProgram(m_program_draw);
        glBindVertexArray(m_vao);

        // Transform inv= Inverse(m_camera.viewport() * m_camera.projection() * m_camera.view());
        program_uniform(m_program_draw, "invMatrix", (player_transform));
        program_uniform(m_program_draw, "camera_position", Inverse(v)(Point(0, 0, 0)));

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
        program_uniform(m_program_draw, "texture0", int(0));    
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        return 1;
    }

protected:
    GLuint mesh_program = read_program(
        smart_path("data/shaders/mesh.glsl"));
    GLuint mesh_color_program = read_program(
        smart_path("data/shaders/mesh_color.glsl"),
        "#define USE_COLOR\n");
    GLuint m_vao;
    GLuint m_texture = read_cubemap(0, "data/cubemap/space.png");
    GLuint m_program_draw = read_program("tutos/draw_cubemap.glsl");

    Transform camera;

    Orbiter m_camera;

    float angle = 0.f;
    float time = 0.f;

    const unsigned NB_POINTS = 1000;
    const unsigned STEP = 50;
    unsigned ra=0, rb=0, rc=0;

    Tube c;

    std::unique_ptr<Mesh> curve_mesh;
    std::unique_ptr<Mesh> debug_mesh;

    std::vector<CylindricalCoordinates> obstacles;
    Player player;

    Box player_collider = {};

    Mesh obstable_mesh = read_mesh(smart_path("data/cube.obj"));

    Transform player_transform;
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
