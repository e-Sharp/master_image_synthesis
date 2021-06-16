#pragma once

#include "src/student/transform/all.hpp"
#include "src/student/vector/all.hpp"
#include "src/student/chaikin.hpp"

#include "src/gKit/mat.h"
#include "src/gKit/vec.h"

#include <cstdlib>
#include <vector>

namespace stu {

struct Track {
	Track() {
        unsigned NB_POINTS = 1000;
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
	}

	std::vector<Transform> nodes = {};
};

}
