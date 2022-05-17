#pragma once

#include <string>
#include <vector>
#include "Toks.h"

int main(int argc, char **argv)
{
    std::vector<std::wstring> args(argv + 1, argv + argc);
    iProlog::Toks::main(args);
}

