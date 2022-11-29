#pragma GCC target("avx512f")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include "../state.hpp"

State state;

std::mt19937 rnd{std::random_device{}()};

void cpu_move(std::ofstream& st) {
    std::vector<int> candidate;
    for (int i = 0; i < 64; ++i) if (state.is_putable(state.turn, Position{i})) candidate.emplace_back(i);
    Position pos{candidate[rnd() % candidate.size()]};
    st << (char)('a' + pos.y) << pos.x + 1;
    state.move(pos);
}

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    int first = atoi(argv[1]), last = atoi(argv[2]);
    for (int f = first; f < last; ++f) {
        std::ofstream st{"self_play/" + std::to_string(f) + ".txt"};
        for (int i = 0; i < 10000; ++i) {
            state = State{};
            for (; ; ) {
                if (!state.any_putable(state.turn)) {
                    if (!state.any_putable(state.turn.next())) break;
                    state.pass();
                    continue;
                }
                cpu_move(st);
            }
            st << std::endl;
        }
    }
}
