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

struct State {
    // ターンのクラス
    struct Turn {
        // falseの時黒
        bool color;

        Turn() noexcept = default;
        Turn(bool color) noexcept: color(color) {}

        // boolに変換
        explicit operator bool() const noexcept {
            return color;
        }

        // デバッグ用
        std::string str() const {
            return color ? "white" : "black";
        }

        // 黒か
        bool is_black() const noexcept {
            return !color;
        }

        // 白か
        bool is_white() const noexcept {
            return color;
        }

        // 比較
        bool operator ==(const Turn& stone) const noexcept {
            return color == stone.color;
        }
        bool operator !=(const Turn& stone) const noexcept {
            return color != stone.color;
        }

        // 次のターン
        void toggle() noexcept {
            color = !color;
        }

        // 次のターンを返す
        Turn next() const noexcept {
            return Turn{!color};
        }
    };

    inline static const Turn black{false}, white{true};

    // 位置を表すクラス
    struct Position {
        int x, y;

        Position() noexcept = default;
        Position(int x, int y) noexcept: x(x), y(y) {}
        explicit Position(int t) noexcept: x(t >> 3), y(t & 7) {}

        // 比較
        bool operator ==(const Position& pos) const noexcept {
            return x == pos.x && y == pos.y;
        }
        bool operator !=(const Position& pos) const noexcept {
            return !(*this == pos);
        }

        // 範囲内か調べる
        bool is_in_range() const noexcept {
            return 0 <= x && x < 8 && 0 <= y && y < 8;
        }

        // bit boardアクセス用
        int flatten() const noexcept {
            return (x << 3) + y;
        }
    };

    // 角
    inline static const std::array<Position, 4> corners{{{0, 0}, {0, 7}, {7, 0}, {7, 7}}};

    // 方向を表すクラス
    struct Direction {
        int dx, dy;

        Direction() noexcept = default;
        Direction(int dx, int dy) noexcept: dx(dx), dy(dy) {}

        // 比較
        bool operator ==(const Direction& d) const noexcept {
            return dx == d.dx && dy == d.dy;
        }
        bool operator !=(const Direction& d) const noexcept {
            return !(*this == d);
        }

        // Positionの隣接移動
        friend Position& operator +=(Position& pos, const Direction& d) noexcept {
            pos.x += d.dx;
            pos.y += d.dy;
            return pos;
        }
        friend Position operator +(Position pos, const Direction& d) noexcept {
            return pos += d;
        }
    };

    // 8方向
    inline static const std::array<Direction, 8> around{{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

    // 二つのbit boardで盤面を表す
    using Board = std::array<std::uint64_t, 2>;

    // 現在のターン
    Turn turn;
    // board[0]に黒のbit board, board[1]に白のbit board
    Board board;

    // 初期盤面
    State(): turn{}, board{} {
        put(black, {3, 4});
        put(black, {4, 3});
        put(white, {3, 3});
        put(white, {4, 4});
    }

    // sが存在するか調べる
    bool exists(Turn turn, const Position& pos) const {
        return board[(bool)turn] >> pos.flatten() & 1;
    }

    // posが空白か調べる
    bool is_blank(const Position& pos) const {
        return !(board[0] >> pos.flatten() & 1) && !(board[1] >> pos.flatten() & 1);
    }

    // posにturnの石を置く
    void put(Turn turn, const Position& pos) {
        board[(bool)turn] |= 1ULL << pos.flatten();
        board[!(bool)turn] &= ~(1ULL << pos.flatten());
    }

    // 石の数を調べる
    int count_stone(Turn turn) const {
        return __builtin_popcountll(board[(bool)turn]);
    }

    // 全ての石の数を調べる
    int count_all_stone() const {
        return count_stone(Turn{false}) + count_stone(Turn{true});
    }

    // sを置いたときにd方向に裏返せるか
    bool is_flippable(Turn turn, Position pos, const Direction& d) const {
        pos += d;
        if (!(pos.is_in_range() && exists(turn.next(), pos))) return false;
        do {
            pos += d;
        } while (pos.is_in_range() && exists(turn.next(), pos));
        return pos.is_in_range() && exists(turn, pos);
    }

    // sを置けるか
    bool is_putable(Turn turn, const Position& pos) const {
        if (!is_blank(pos)) return false;
        for (const auto& d: around) if (is_flippable(turn, pos, d)) return true;
        return false;
    }

    // 石をどこかに置けるか
    bool any_putable(Turn turn) const {
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) if (is_putable(turn, {i, j})) return true;
        return false;
    }

    // 終了盤面か調べる
    bool has_ended() const {
        return !any_putable(turn) && any_putable(turn.next());
    }

    // d方向を裏返す
    void flip(Turn turn, Position pos, const Direction& d) {
        for (pos += d; exists(turn.next(), pos); pos += d) {
            put(turn, pos);
        }
    }

    // posに石を置く
    void move(const Position& pos) {
        put(turn, pos);
        for (const auto& d: around) {
            if (is_flippable(turn, pos, d)) flip(turn, pos, d);
        }
        turn.toggle();
    }

    // posに石を置いた後のStateを返す
    State moved(const Position& pos) const {
        State res = *this;
        res.move(pos);
        return res;
    }

    // パスする
    void pass() noexcept {
        turn.toggle();
    }

    // パスした後のStateを返す
    State passed() const noexcept {
        State res = *this;
        res.pass();
        return res;
    }

    // 石差を調べる
    int stone_diff() const {
        return count_stone(turn) - count_stone(!turn);
    }

    // 角マスの評価値
    inline static const std::array<std::array<int, 8>, 8> board_eval{{
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
                if (is_putable(turn.next(), {i, j})) return passed().moved({i, j}).stone_diff();
            }
        }
        if (cnt >= 57) {
            res += 10 * (count_stone(turn) - count_stone(turn.next()));
            for (const auto& pos: corners) {
                Direction dx{}, dy{};
                if (pos.x == 0) dx.dx = 1;
                else dx.dx = -1;
                if (pos.y == 0) dy.dy = 1;
                else dy.dy = -1;
                if (exists(turn, pos)) {
                    res += 8;
                    for (Position p = pos + dx; p.is_in_range() && exists(turn, p); p += dx) res += 8;
                    for (Position p = pos + dy; p.is_in_range() && exists(turn, p); p += dy) res += 8;
                } else if (exists(turn.next(), pos)) {
                    res -= 8;
                    for (Position p = pos + dx; p.is_in_range() && exists(turn.next(), p); p += dx) res -= 8;
                    for (Position p = pos + dy; p.is_in_range() && exists(turn.next(), p); p += dy) res -= 8;
                }
            }
            if (!any_putable(turn)) res -= 80;
            return res;
        }
        res = 0;
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) {
            if (exists(turn, {i, j})) res += board_eval[i][j];
            else if (exists(turn.next(), {i, j})) res -= board_eval[i][j];
        }
        for (const auto& pos: corners) {
            Direction dx{}, dy{};
            if (pos.x == 0) dx.dx = 1;
            else dx.dx = -1;
            if (pos.y == 0) dy.dy = 1;
            else dy.dy = -1;
            if (exists(turn, pos)) {
                for (Position p = pos + dx; p.is_in_range() && exists(turn, p); p += dx) res += 10 - board_eval[p.x][p.y];
                for (Position p = pos + dy; p.is_in_range() && exists(turn, p); p += dy) res += 10 - board_eval[p.x][p.y];
            } else if (exists(turn.next(), pos)) {
                for (Position p = pos + dx; p.is_in_range() && exists(turn.next(), p); p += dx) res -= 10 - board_eval[p.x][p.y];
                for (Position p = pos + dy; p.is_in_range() && exists(turn.next(), p); p += dy) res -= 10 - board_eval[p.x][p.y];
            }
        }
        bool flag = true;
        for (int i = 0; i < 64; ++i) if (is_putable(turn, Position{i})) {
            flag = false;
            res += 15;
        }
        if (flag) res -= 80;
        for (int i = 0; i < 64; ++i) if (is_putable(turn.next(), Position{i})) {
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
    bool operator ==(const State& state) const noexcept {
        return turn == state.turn && board == state.board;
    }
    bool operator !=(const State& state) const noexcept {
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
