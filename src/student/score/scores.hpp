#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm> 
#include <memory>

namespace stu {

struct Scores {
    Scores() {
        v = std::make_shared<std::vector<int>>();
    }

    std::shared_ptr<std::vector<int>> load() {
        std::ifstream scores;
        scores.open(filename);
        if (scores.is_open()) {
            int s;
            while (scores >> s)
                v->push_back(int(s));
            scores.close();
        }
        return v;
    }

    std::shared_ptr<std::vector<int>> getScores() {
        return v;        
    }

    void add(int s) {
        v->push_back(s);
        std::sort(v->begin(), v->end(), std::greater<>());
    }

    void save() {
        std::ofstream scores;
        scores.open(filename);   
        if (scores.is_open()) {
            for(const auto& s: (*v))
                scores << s << ' ';
            scores.close();
        }
    }

    std::shared_ptr<std::vector<int>> v;
    const std::string filename = "scores.sc";
    const std::size_t NB = 10;
};

}
