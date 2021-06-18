#pragma once

#include "src/student/transform/all.hpp"
#include "src/student/vector/all.hpp"
#include "src/student/chaikin.hpp"
#include "src/student/mod.hpp"

#include "src/gKit/mat.h"
#include "src/gKit/program.h"
#include "src/gKit/uniforms.h"
#include "src/gKit/vec.h"

#include <cstdlib>
#include <iostream>
#include <vector>

namespace stu {

struct Track {
	Track() {
        unsigned NB_POINTS = 100;
        unsigned STEP = 50;
        unsigned ra = 0, rb = 0, rc = 0;
		for(std::size_t i = 0; i < NB_POINTS; ++i) {
            ra += std::rand() % STEP;
            rb += std::rand() % STEP;
            rc += std::rand() % STEP;
            nodes.push_back(Translation(ra, rb, rc));
        }

        chaikin(nodes);
        chaikin(nodes);
        chaikin(nodes);
        chaikin(nodes);

        // compute_forward(c);
        if(nodes.size() < 2) throw std::logic_error("");
        fw(nodes[0], normalize(pos(nodes[1]) - pos(nodes[0])));
        for (std::size_t i = 2; i < nodes.size(); ++i) {
            fw(nodes[i - 1], normalize(pos(nodes[i]) - pos(nodes[i - 2])));
        }
        fw(nodes[nodes.size() - 1], normalize(pos(nodes[nodes.size() - 1]) - pos(nodes[nodes.size() - 2])));

        // compute_right(c);
        right(nodes.front(), normalize(perpendicular(fw(nodes.front()))));
        for(std::size_t i = 1; i < nodes.size(); ++i) {
            auto& n0 = nodes[i - 1];
            auto& n1 = nodes[i];

            auto r = rotation(fw(n0), fw(n1));
            right(n1, normalize(r(right(n0))));
        }

        // compute_up(c);
        for(auto& n : nodes) {
            up(n, cross(right(n), fw(n)));
        }

        generate_mesh(15);
	}

    void generate_mesh(unsigned resolution) {
        mesh = Mesh(GL_TRIANGLES);
        auto circle = std::vector<Point>(resolution + 1);
        for(unsigned i = 0; i < resolution; ++i) {
            circle[i] = RotationX(360.f / resolution * i)(Point(0, 1, 0));
        }
        circle.back() = circle.front();
        for(unsigned i = 0; i < nodes.size() - 1; ++i)
        for(unsigned j = 0; j <= 1; ++j) {
            const auto& n = nodes[i + j];
            for(unsigned k = 0; k <= resolution; ++k) {
                mesh.normal(n(Vector(circle[k])));
                mesh.texcoord(float(k) / resolution, (mod(float(i), 12.f) + j) / 12.f);
                mesh.vertex(n(circle[k]));
            }
        }
        for(unsigned i = 0; i < nodes.size() - 1; ++i)
        for(unsigned j = 0; j <= resolution - 1; ++j) {
            auto v1 = 2 * i * (resolution + 1) + j;
            auto v2 = 2 * i * (resolution + 1) + (j + 1);
            auto v3 = (2 * i + 1) * (resolution + 1) + j;
            auto v4 = (2 * i + 1) * (resolution + 1) + (j + 1);
            mesh.triangle(v1, v2, v4);
            mesh.triangle(v1, v4, v3);
        }
    }

    void draw(Transform m, Transform v, Transform p) {
        auto mv = v * m;
        auto mvp = p * mv;
        glUseProgram(shader);
        program_uniform(shader, "mesh_color", Color(1, 1, 1, 1));
        program_uniform(shader, "mvpMatrix", mvp);
        program_uniform(shader, "normalMatrix", mv);
        program_use_texture(shader, "diffuse_color", 0, texture);
        mesh.draw(shader, true, true, true, false, false);
    }

	std::vector<Transform> nodes = {};

    Mesh mesh;
    GLuint shader = read_program(
        smart_path("data/shaders/mesh.glsl"),
        "#define USE_NORMAL\n"
        "#define USE_TEXCOORD\n");
    GLuint texture = read_texture(0,
        smart_path("data/scales.png"));
};

}
