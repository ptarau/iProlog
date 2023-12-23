#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string file2string(std::string path) {
    std::ifstream f(path);
    if (!f.good())
        throw std::invalid_argument(path + " not found");
    std::stringstream s;
    s << f.rdbuf();
    return s.str();
}
