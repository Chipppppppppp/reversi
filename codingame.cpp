#pragma GCC target("avx512f")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <unordered_map>
#include "state.hpp"

State state;

std::unordered_map<State, Position> book;

struct BookInit {
    static constexpr const char* book_str = "f5\nf5d6\nf5d6c3g5\nf5d6c3g5c6c5\nf5d6c3g5c6c5c4b6\nf5d6c3g5c6c5c4b6f6f4\nf5d6c3g5c6c5c4b6f6f4e6d7\nf5d6c3g5c6c5c4b6f6f4e6d7c7g6\nf5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5\nf5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3\nf5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3a6e3\nf5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3a6e3a5d3\nf5d6c3g5f6d3\nf5d6c3g5f6d3e3c2\nf5d6c3g5f6d3e3c2c1e6\nf5d6c3g5f6d3e3c2c1e6f4f3\nf5d6c3g5f6d3e3c2c1e6f4f3f2g4\nf5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2\nf5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4\nf5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4h5f7\nf5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4h5f7e7g3\nf5d6c3g5g6d3\nf5d6c3g5g6d3c4e3\nf5d6c3g5g6d3c4e3f3b4\nf5d6c3g5g6d3c4e3f3b4f6e6\nf5d6c3g5g6d3c4e3f3b4f6e6f4g4\nf5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5\nf5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3\nf5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3h3f7\nf5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3h3f7f8c2\nf5d6c4b3\nf5d6c4b3b4f4\nf5d6c4b3b4f4f6g5\nf5d6c4b3b4f4f6g5f3e7\nf5d6c4b3b4f4f6g5f3e7c5e6\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4d3\nf5d6c5b4\nf5d6c5b4d7e7\nf5d6c5b4d7e7c7d8\nf5d6c5b4d7e7c7d8c3d3\nf5d6c5b4d7e7c7d8c3d3c4b3\nf5d6c5b4d7e7c7d8c3d3c4b3d2e2\nf5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3\nf5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2\nf5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2c6b5\nf5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2c6b5f3c8\nf5d6c4\nf5d6c4b3b4\nf5d6c4b3b4f4f6\nf5d6c4b3b4f4f6g5f3\nf5d6c4b3b4f4f6g5f3e7c5\nf5d6c4b3b4f4f6g5f3e7c5e6c3\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4\nf5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4d3e2\nf5d6c4d3c3\nf5d6c4d3c3b3d2\nf5d6c4d3c3b3d2e1b5\nf5d6c4d3c3b3d2e1b5c5b4\nf5d6c4d3c3b3d2e1b5c5b4e3c2\nf5d6c4d3c3b3d2e1b5c5b4e3c2a4c6\nf5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2\nf5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6\nf5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6f1e6\nf5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6f1e6f3f2\nf5d6c4d3c3f4f6\nf5d6c4d3c3f4f6f3e6\nf5d6c4d3c3f4f6f3e6e7f7\nf5d6c4d3c3f4f6f3e6e7f7c5b6\nf5d6c4d3c3f4f6f3e6e7f7c5b6g5e3\nf5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6\nf5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4\nf5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4h3d2\nf5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4h3d2g3f1\nf5d6c4d3c3f4f6f3e6e7f7c5b6g6e3\nf5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1\nf5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5\nf5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5c6d8\nf5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5c6d8g4h6\nf5d6c4d3c3f4f6b4c2\nf5d6c4d3c3f4f6b4c2f3e3\nf5d6c4d3c3f4f6b4c2f3e3e2c6\nf5d6c4d3c3f4f6b4c2f3e3e2c6f2c5\nf5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2\nf5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7\nf5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7b3g5\nf5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7b3g5c8h4\nf5d6c4d3c3f4f6g5e3\nf5d6c4d3c3f4f6g5e3f3g6\nf5d6c4d3c3f4f6g5e3f3g6e2h5\nf5d6c4d3c3f4f6g5e3f3g6e2h5c5g4\nf5d6c4d3c3f4f6g5e3f3g6e2h5c5g4g3f2\nf5d6c4d3c3b5b4\nf5d6c4d3c3b5b4f4c5\nf5d6c4d3c3b5b4f4c5a4b3\nf5d6c4d3c3b5b4f4c5a4b3d2a6\nf5d6c4d3c3b5b4f4c5a4b3d2a6a3e3\nf5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4\nf5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6\nf5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6g3e2\nf5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6g3e2c2f2\nf5d6c4g5f6\nf5d6c4g5f6f4f3\nf5d6c4g5f6f4f3d3c3\nf5d6c4g5f6f4f3d3c3g6e3\nf5d6c4g5f6f4f3d3c3g6e3e6h5\nf5d6c4g5f6f4f3d3c3g6e3e6h5d2e2\nf5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6\nf5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6\nf5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6b4b3\nf5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6b4b3c7a4\nf5f6e6\nf5f6e6f4g6\nf5f6e6f4g6c5f3\nf5f6e6f4g6c5f3g4e3\nf5f6e6f4g6c5f3g4e3d6g5\nf5f6e6f4g6c5f3g4e3d6g5g3c3\nf5f6e6f4g6c5f3g4e3d6g5g3c3h5c4\nf5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6\nf5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3\nf5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3f7e7\nf5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3f7e7f8h4\nf5f6e6f4g6c5f3g5d6\nf5f6e6f4g6c5f3g5d6e3h4\nf5f6e6f4g6c5f3g5d6e3h4g3g4\nf5f6e6f4g6c5f3g5d6e3h4g3g4h6e2\nf5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5\nf5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6\nf5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6e7f2\nf5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6e7f2c4d2\nf5f6e6f4g6d6g4\nf5f6e6f4g6d6g4g5h4\nf5f6e6f4g6d6g4g5h4e7f3\nf5f6e6f4g6d6g4g5h4e7f3h6f7\nf5f6e6f4g6d6g4g5h4e7f3h6f7e8f8\nf5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3\nf5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7\nf5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7e3c5\nf5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7e3c5c4g3\nf5f6e6d6f7\nf5f6e6d6f7e3c6\nf5f6e6d6f7e3c6e7f4\nf5f6e6d6f7e3c6e7f4c5d8\nf5f6e6d6f7e3c6e7f4c5d8c7d7\nf5f6e6d6f7e3c6e7f4c5d8c7d7f8b5\nf5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8\nf5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3\nf5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3g5b6\nf5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3g5b6d3b4\nf5f6e6d6f7f4d7\nf5f6e6d6f7f4d7e7d8\nf5f6e6d6f7f4d7e7d8g5c6\nf5f6e6d6f7f4d7e7d8g5c6f8g6\nf5f6e6d6f7f4d7e7d8g5c6f8g6h5h6\nf5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4\nf5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8\nf5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8c5e3\nf5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8c5e3d3c7\n";
    BookInit() {
        std::stringstream st(book_str);
        std::string line;
        State state;
        int n = 0;
        int x, y;
        for (; ; ) {
            std::getline(st, line);
            if (st.eof()) break;
            ++n;
            state = State{};
            for (int i = 0; i < (int)line.size() - 2; i += 2) {
                x = (int)(line[i]) - (int)'a';
                y = (int)(line[i + 1]) - (int)'1';
                state.move({x, y});
            }
            x = (int)(line[line.size() - 2]) - (int)'a';
            y = (int)(line[line.size() - 1]) - (int)'1';
            book[state] = {x, y};

            state = State{};
            for (int i = 0; i < (int)line.size() - 2; i += 2) {
                x = (int)(line[i]) - (int)'a';
                y = (int)(line[i + 1]) - (int)'1';
                state.move({y, x});
            }
            x = (int)(line[line.size() - 2]) - (int)'a';
            y = (int)(line[line.size() - 1]) - (int)'1';
            book[state] = {y, x};

            state = State{};
            for (int i = 0; i < (int)line.size() - 2; i += 2) {
                x = (int)(line[i]) - (int)'a';
                y = (int)(line[i + 1]) - (int)'1';
                state.move({7 - x, 7 - y});
            }
            x = (int)(line[line.size() - 2]) - (int)'a';
            y = (int)(line[line.size() - 1]) - (int)'1';
            book[state] = {7 - x, 7 - y};

            state = State{};
            for (int i = 0; i < (int)line.size() - 2; i += 2) {
                x = (int)(line[i]) - (int)'a';
                y = (int)(line[i + 1]) - (int)'1';
                state.move({7 - y, 7 - x});
            }
            x = (int)(line[line.size() - 2]) - (int)'a';
            y = (int)(line[line.size() - 1]) - (int)'1';
            book[state] = {7 - y, 7 - x};
        }
    }
} book_init;

clock_t start_time;
int result_a, result_b;
template<int time_limit, bool first = true>
int alpha_beta(const State& state, int depth, int a, int b, Position& pos) {
    if (clock() - start_time >= time_limit) return 0;
    if (depth == 0) return state.eval();
    bool flag = true;
    for (int i = 0; i < 64; ++i) if (Position p{i}; state.is_putable(state.turn, p)) {
        flag = false;
        int res = -alpha_beta<time_limit, false>(state.moved(p), depth - 1, -b, -a, pos);
        if (clock() - start_time >= time_limit) return 0;
        if (a < res) {
            a = res;
            if constexpr (first) pos = p;
        }
        if (a >= b) return a;
    }
    if (flag) {
        if (state.any_putable(state.turn.next())) return -alpha_beta<time_limit, false>(state.passed(), depth - 1, -b, -a, pos);
        else return state.eval();
    }
    return a;
}

constexpr int time_limit = CLOCKS_PER_SEC / 8;
constexpr int last_stage_time_limit = time_limit / 2;
std::pair<Position, int> alpha_beta_move() noexcept {
    start_time = clock();
    Position pos;
    int res;
    if (state.count_all_stone() >= 54) {
        res = alpha_beta<last_stage_time_limit>(state, inf, -inf, inf, pos);
        if (clock() - start_time < last_stage_time_limit) return {pos, res};
    }
    Position pos_before;
    int res_before;
    for (int depth = 3; ; ++depth) {
        res = alpha_beta<time_limit>(state, depth, -inf, inf, pos);
        if (clock() - start_time >= time_limit) return {pos_before, res_before};
        pos_before = pos;
        res_before = res;
    }
}

void cpu_move() {
    Position pos;
    auto itr = book.find(state);
    if (itr != book.end()) {
        pos = itr->second;
        std::cout << (char)('a' + pos.y) << pos.x + 1 << " MSG " << "formula" << '\n';
    } else {
        auto [pos, res] = alpha_beta_move();
        std::cout << (char)('a' + pos.y) << pos.x + 1 << " MSG " << res << '\n';
    }
    fflush(stdout);
    state.move(pos);
}

void input_move() {
    std::string s;
    bool flag = false;
    for (int i = 0; i < 8; ++i) {
        std::cin >> s;
        for (int j = 0; j < 8; ++j) if (s[j] != '.' && !state.exists(black, {i, j}) && !state.exists(white, {i, j})) {
            state.move({i, j});
            state.pass();
            flag = true;
        }
    }
    if (flag) state.pass();
    int action_count;
    std::cin >> action_count;
    for (int i = 0; i < action_count; ++i) std::cin >> s;
}

int main() {
    bool my_turn;
    std::cin >> my_turn;
    std::string s;
    std::cin >> s;
    if (!my_turn) {
        for (int i = 0; i < 8; ++i) std::cin >> s;
        int action_count;
        std::cin >> action_count;
        for (int i = 0; i < action_count; ++i) std::cin >> s;
    }
    for (; ; ) {
        if (!state.any_putable(state.turn)) {
            if (!state.any_putable(state.turn.next())) break;
            state.pass();
            continue;
        }
        if ((bool)state.turn == my_turn) cpu_move();
        else input_move();
    }
}
