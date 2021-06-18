
#include "texture.h"
#include "wavefront.h"

#include "src/student/all.hpp"
#include "app.h"
#include "draw.h"
#include "program.h"
#include "orbiter.h"
#include "uniforms.h"
#include "text.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>
#include <ctime>

using namespace stu;

Mesh Obstacle::mesh = read_mesh(smart_path("data/cube.obj"));

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

class TP : public App {
public:
    TP() : App(1280, 720) {}

    int init() {
        program_uniform(m_program_draw, "ww", 1280);
        program_uniform(m_program_draw, "wh", 720);

        glGenVertexArrays(1, &m_vao);

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);

        glClearDepth(1.f);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        unsigned nb_obstacles = 50;
        unsigned co = 20;
        for(std::size_t i = 0; i < nb_obstacles; ++i) {
            if(std::rand() % 2) {
                gear(std::back_inserter(obstacles), co, 3);
                co += std::rand() % 10 + 5;
            } else {
                rotor(std::back_inserter(obstacles), co, 10);
                co += std::rand() % 20 + 10;
            }
        }

        unsigned nb_coins = 600;
        coins.resize(nb_coins);
        co = 20;
        unsigned d = 0;
        for(std::size_t i = 0; i < nb_coins; ++i) {
            auto& o = coins[i];
            o.coords.azimuth = float((std::rand() % 12) * 30);
            o.coords.coordinate = float(co);
            o.coords.radius = 2.f;
            co += std::rand() % 10 + 10;
        }

        display = create_text();
        scores.load();
        return 0;
    }

    int quit() {
        release_text(display);
        return 0;
    }

    int update(float t, float dt) override {
        if(key_state(SDLK_DOWN)) {
            player.brake();
        }
        if(key_state(SDLK_LEFT)) {
            player.turn_left();
        }
        if(key_state(SDLK_RIGHT)) {
            player.turn_right();
        }
        if(key_state(SDLK_UP)) {
            player.accelerate();
        }

        player.update();

        auto pt = transform(track, player.coords);
        camera = Lookat(
            pos(pt) - 5.f * fw(pt) + 3.f * up(pt),
            pos(pt),
            up(pt));


        player_collider.T = pt;
        player_transform = pt;

        for(auto& o : obstacles) {
            o.update(t);
        }

        sc = player.coords.coordinate + sc_coins;

        return 0;
    }

    int display_text() {
        clear(display);
        printf(display, 0, 0, "Score: %i", sc);
        for(std::size_t i=0; i<std::min(scores.NB, scores.getScores()->size()); ++i)
            printf(display, 0, i+2, "Top %i: %i", i+1, scores.getScores()->at(i));

        draw(display, window_width(), window_height());

        return 0;
    }

    void loose() {
        scores.add(sc);
        scores.save();
        sc = 0;
        sc_coins = 0;
        player.reset();
        for(auto& c : coins) {
            c.touched = false;
        }
    }

    int render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Transform v = camera;
        Transform p = Perspective(90.f, 16.f / 9.f, 1.f, 1000.f);
        Transform vp = p * v;
    
        track.draw(Identity(), v, p);

        for(const auto& o : obstacles) {
            auto model = transform(track, o.coords);
            auto collider = Box();
            collider.T = model;

            glUseProgram(mesh_program);
            if(collides(collider, player_collider)) {
                program_uniform(mesh_program, "mesh_color", Color(1, 0, 0, 1));
                loose();
            } else {
                program_uniform(mesh_program, "mesh_color", Color(0, 1, 0, 1));
            }
            program_uniform(mesh_program, "mvMatrix", v * model);
            program_uniform(mesh_program, "mvpMatrix", vp * model);
            obstable_mesh.draw(mesh_program, true, false, false, false, false);
        }

        for(auto& c : coins) {
            auto model = transform(track, c.coords);
            auto collider = Box();
            collider.T = model;

            glUseProgram(mesh_program);
            if(collides(collider, player_collider)) {
                program_uniform(mesh_program, "mesh_color", Color(1, 1, 0.5, 1));
                c.touched = true;
                if (!witness) {
                    sc_coins += 20;
                    witness = true;
                    cref = c.coords.coordinate;
                }
            } else {
                if (c.coords.coordinate == cref && witness)
                    witness = false;
                program_uniform(mesh_program, "mesh_color", Color(1, 1, 0, 1));
            }
            program_uniform(mesh_program, "mvMatrix", v * model);
            program_uniform(mesh_program, "mvpMatrix", vp * model);
            if (!c.touched)
                coin_mesh.draw(mesh_program, true, false, false, false, false);
        }

        player.draw(transform(track, player.coords), v, p);

        glUseProgram(m_program_draw);
        glBindVertexArray(m_vao);

        // Transform inv= Inverse(m_camera.viewport() * m_camera.projection() * m_camera.view());
        program_uniform(m_program_draw, "invMatrix", (player_transform));
        program_uniform(m_program_draw, "camera_position", Inverse(v)(Point(0, 0, 0)));

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
        program_uniform(m_program_draw, "texture0", int(0));
        
        glDrawArrays(GL_TRIANGLES, 0, 3);

        display_text();

        return 1;
    }

protected:
    GLuint mesh_program = read_program(
        smart_path("data/shaders/mesh.glsl"));
    GLuint m_vao;
    GLuint m_texture = read_cubemap(0, smart_path("data/cubemap/space.png"));
    GLuint m_program_draw = read_program(smart_path("src/shader/draw_cubemap.glsl"));
    std::vector<Color> m_colors;

    Transform camera;

    Track track = {};

    std::vector<Obstacle> obstacles;
    std::vector<Coin> coins;
    Player player;

    Box player_collider = {};

    Mesh obstable_mesh = read_mesh(smart_path("data/obstacle.obj"));
    Mesh coin_mesh = read_mesh(smart_path("data/coin.obj"));

    Transform player_transform;

    Text display;
    unsigned sc = 0, sc_coins = 0;
    bool witness = false;
    float cref;

    Scores scores;
};

void throwing_main() {
    TP().run();
}

int main(int, char**) {
    std::srand(std::time(nullptr));

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
