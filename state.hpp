#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

// 無限大
inline constexpr int inf = std::numeric_limits<int>::max() / 3;

// falseが黒を表す
inline constexpr bool black = false;
inline constexpr bool white = true;

// 位置を表すクラス
struct Position {
    int x, y;

    Position() noexcept = default;
    constexpr Position(int x, int y) noexcept: x(x), y(y) {}
    constexpr explicit Position(int t) noexcept: x(t >> 3), y(t & 7) {}

    // 比較
    constexpr bool operator ==(const Position& pos) const noexcept {
        return x == pos.x && y == pos.y;
    }
    constexpr bool operator !=(const Position& pos) const noexcept {
        return !(*this == pos);
    }

    // 範囲内か調べる
    constexpr bool is_valid() const noexcept {
        return 0 <= x && x < 8 && 0 <= y && y < 8;
    }

    // bit boardアクセス用
    constexpr int flatten() const noexcept {
        return (x << 3) + y;
    }
};

// 角
constexpr std::array<Position, 4> corners{{{0, 0}, {0, 7}, {7, 0}, {7, 7}}};

// 方向を表すクラス
struct Direction {
    int dx, dy;

    Direction() noexcept = default;
    constexpr Direction(int dx, int dy) noexcept: dx{dx}, dy{dy} {}

    // 比較
    constexpr bool operator ==(const Direction& d) const noexcept {
        return dx == d.dx && dy == d.dy;
    }
    constexpr bool operator !=(const Direction& d) const noexcept {
        return !(*this == d);
    }

    // Positionの隣接移動
    constexpr friend Position& operator +=(Position& pos, const Direction& d) noexcept {
        pos.x += d.dx;
        pos.y += d.dy;
        return pos;
    }
    constexpr friend Position operator +(Position pos, const Direction& d) noexcept {
        return pos += d;
    }
};

// 8方向
constexpr std::array<Direction, 8> around{{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

struct State {
    // 二つのbit boardで盤面を表す
    using Board = std::array<std::uint64_t, 2>;

    // 現在のターン
    bool turn;

    // board[0]に黒のbit board, board[1]に白のbit board
    Board board;

    // 初期盤面
    State(): turn{}, board{} {
        put(black, {3, 4});
        put(black, {4, 3});
        put(white, {3, 3});
        put(white, {4, 4});
    }

    // posが空白か調べる
    bool is_blank(const Position& pos) const {
        return !(board[0] >> pos.flatten() & 1) && !(board[1] >> pos.flatten() & 1);
    }

    // posにturnの石が存在するか調べる
    bool exists(bool turn, const Position& pos) const {
        return board[turn] >> pos.flatten() & 1;
    }

    // posにturnの石を置く
    void put(bool turn, const Position& pos) {
        board[turn] |= 1ULL << pos.flatten();
        board[!turn] &= ~(1ULL << pos.flatten());
    }

    // 石の数を調べる
    int count_stone(bool turn) const {
        return __builtin_popcountll(board[turn]);
    }

    // 石差を調べる
    int stone_diff() const {
        return count_stone(turn) - count_stone(!turn);
    }

    // 全ての石の数を調べる
    int count_all_stone() const {
        return count_stone(black) + count_stone(white);
    }

    // posにturnの石を置いたときにd方向に裏返せるか
    bool is_flippable(bool turn, Position pos, const Direction& d) const {
        pos += d;
        if (!(pos.is_valid() && exists(!turn, pos))) return false;
        do {
            pos += d;
        } while (pos.is_valid() && exists(!turn, pos));
        return pos.is_valid() && exists(turn, pos);
    }

    // posにturnの石を置けるか
    bool is_putable(bool turn, const Position& pos) const {
        if (!is_blank(pos)) return false;
        for (const auto& d: around) if (is_flippable(turn, pos, d)) return true;
        return false;
    }

    // 石をどこかに置けるか
    bool any_putable(bool turn) const {
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) if (is_putable(turn, {i, j})) return true;
        return false;
    }

    // 終了盤面か調べる
    bool has_ended() const {
        return !any_putable(black) && !any_putable(white);
    }

    // d方向を裏返す
    void flip(bool turn, Position pos, const Direction& d) {
        for (pos += d; exists(!turn, pos); pos += d) {
            put(turn, pos);
        }
    }

    // posに石を置く
    void move(const Position& pos) {
        put(turn, pos);
        for (const auto& d: around) {
            if (is_flippable(turn, pos, d)) flip(turn, pos, d);
        }
        turn = !turn;
    }

    // posに石を置いた後のStateを返す
    State moved(const Position& pos) const {
        State res = *this;
        res.move(pos);
        return res;
    }

    // パスする
    void pass() noexcept {
        turn = !turn;
    }

    // パスした後のStateを返す
    State passed() const noexcept {
        State res = *this;
        res.pass();
        return res;
    }

    // 角マスの評価値
    static constexpr std::array<std::array<int, 8>, 8> board_eval{{
        {120, -40, 20, 5, 5, 20, -40, 120},
        {-40, -80, -1, -1, -1, -1, -80, -40},
        {20, -1, 5, 1, 1, 5, -1, 20},
        {5, -1, 1, 0, 0, 1, -1, 5},
        {5, -1, 1, 0, 0, 1, -1, 5},
        {20, -1, 5, 1, 1, 5, -1, 20},
        {-40, -80, -1, -1, -1, -1, -80, -40},
        {120, -40, 20, 5, 5, 20, -40, 120}
    }};

    // 評価関数
    int eval() const {
        if (has_ended()) return stone_diff();
        int res = 0;
        int cnt = count_all_stone();
        if (cnt == 63) {
            for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) {
                if (is_putable(turn, {i, j})) return -moved({i, j}).stone_diff();
                if (is_putable(!turn, {i, j})) return passed().moved({i, j}).stone_diff();
            }
        }
        if (cnt >= 57) {
            res += 10 * (count_stone(turn) - count_stone(!turn));
            for (const auto& pos: corners) {
                Direction dx{}, dy{};
                if (pos.x == 0) dx.dx = 1;
                else dx.dx = -1;
                if (pos.y == 0) dy.dy = 1;
                else dy.dy = -1;
                if (exists(turn, pos)) {
                    res += 8;
                    for (Position p = pos + dx; p.is_valid() && exists(turn, p); p += dx) res += 8;
                    for (Position p = pos + dy; p.is_valid() && exists(turn, p); p += dy) res += 8;
                } else if (exists(!turn, pos)) {
                    res -= 8;
                    for (Position p = pos + dx; p.is_valid() && exists(!turn, p); p += dx) res -= 8;
                    for (Position p = pos + dy; p.is_valid() && exists(!turn, p); p += dy) res -= 8;
                }
            }
            if (!any_putable(turn)) res -= 80;
            return res;
        }
        res = 0;
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) {
            if (exists(turn, {i, j})) res += board_eval[i][j];
            else if (exists(!turn, {i, j})) res -= board_eval[i][j];
        }
        for (const auto& pos: corners) {
            Direction dx{}, dy{};
            if (pos.x == 0) dx.dx = 1;
            else dx.dx = -1;
            if (pos.y == 0) dy.dy = 1;
            else dy.dy = -1;
            if (exists(turn, pos)) {
                for (Position p = pos + dx; p.is_valid() && exists(turn, p); p += dx) res += 10 - board_eval[p.x][p.y];
                for (Position p = pos + dy; p.is_valid() && exists(turn, p); p += dy) res += 10 - board_eval[p.x][p.y];
            } else if (exists(!turn, pos)) {
                for (Position p = pos + dx; p.is_valid() && exists(!turn, p); p += dx) res -= 10 - board_eval[p.x][p.y];
                for (Position p = pos + dy; p.is_valid() && exists(!turn, p); p += dy) res -= 10 - board_eval[p.x][p.y];
            }
        }
        bool flag = true;
        for (int i = 0; i < 64; ++i) if (is_putable(turn, Position{i})) {
            flag = false;
            res += 15;
        }
        if (flag) res -= 80;
        for (int i = 0; i < 64; ++i) if (is_putable(!turn, Position{i})) {
            res -= 15;
        }
        return res;
    }

    void show() const {
        std::cerr << "  ";
        for (int i = 0; i < 8; ++i) std::cerr << (char)('a' + i) << " \n"[i == 7];
        for (int i = 0; i < 8; ++i) {
            std::cerr << i + 1 << ' ';
            for (int j = 0; ; ++j) {
                if (exists(black, {i, j})) std::cerr << '0';
                else if (exists(white, {i, j})) std::cerr << '1';
                else if (is_putable(turn, {i, j})) std::cerr << '.';
                else std::cerr << ' ';
                if (j != 7) std::cerr << ' ';
                else {
                    std::cerr << '\n';
                    break;
                }
            }
        }
    }

    // 比較関数
    constexpr bool operator ==(const State& state) const noexcept {
        return turn == state.turn && board == state.board;
    }
    constexpr bool operator !=(const State& state) const noexcept {
        return !(*this == state);
    }
};

// ハッシュ
template<>
struct std::hash<State> {
private:
    std::hash<std::uint64_t> hasher;
public:
    std::size_t operator ()(const State& state) const noexcept {
        std::size_t seed = (bool)state.turn;
        seed ^= hasher(state.board[0]) + 0x9e3779b97f4a7c15LU + (seed << 12) + (seed >> 4);
        seed ^= hasher(state.board[1]) + 0x9e3779b97f4a7c15LU + (seed << 12) + (seed >> 4);
        return seed;
    }
};
