#include <QRandomGenerator>

#include "boardmodel.h"

BoardModel::BoardModel()
    : _board(_get_solved_board()),
      _nul_index(GAME_SIZE - 1),
      _is_solved(true),
      _start_nul_index(_nul_index),
      _start_is_solved(_is_solved),
      _num_shifts(0),
      _states{} {}

BoardModel::BoardModel(bool is_rnd, int complexity)
    : BoardModel() {
    if (is_rnd) {
        _states.reserve(complexity * _COMPLEXITY_COEF);

        while (_is_solved) {
            const auto rnd_result = _gen_board(complexity * _COMPLEXITY_COEF);
            const auto board      = rnd_result.first;
            const auto nul_index  = rnd_result.second;
            const auto is_solved  = _check_solved(board);

            if (!is_solved) {
                _is_solved   = is_solved;
                _board       = board;
                _start_board = _board;
                _nul_index   = nul_index;
            }
        }
    }
}

QPair<bool, QVariant> BoardModel::is_solved() const noexcept {
    if (_is_solved) {
        return {true, QVariant(_num_shifts)};
    } else {
        return {false, QVariant()};
    }
}

const QVector<int>& BoardModel::get_board() const noexcept {
    return _board;
}

void BoardModel::set_start_board() {
    if (_num_shifts != 0) {
        _board      = _start_board;
        _nul_index  = _start_nul_index;
        _is_solved  = _start_is_solved;
        _num_shifts = 0;
        _states.clear();
    }
}

QPair<bool, QVariant> BoardModel::move(int idx) {
    bool result = false;
    int swap_idx;

    if (_check_direction(_nul_index, DIRECTION::UP)) {
        if (_check_is_neighbour(_nul_index, idx, DIRECTION::UP)) {
            swap_idx = _nul_index;
            _make_move(_board, _nul_index, DIRECTION::UP);
            result = true;
        }
    }

    if (_check_direction(_nul_index, DIRECTION::DOWN)) {
        if (_check_is_neighbour(_nul_index, idx, DIRECTION::DOWN)) {
            swap_idx = _nul_index;
            _make_move(_board, _nul_index, DIRECTION::DOWN);
            result = true;
        }
    }

    if (_check_direction(_nul_index, DIRECTION::LEFT)) {
        if (_check_is_neighbour(_nul_index, idx, DIRECTION::LEFT)) {
            swap_idx = _nul_index;
            _make_move(_board, _nul_index, DIRECTION::LEFT);
            result = true;
        }
    }

    if (_check_direction(_nul_index, DIRECTION::RIGHT)) {
        if (_check_is_neighbour(_nul_index, idx, DIRECTION::RIGHT)) {
            swap_idx = _nul_index;
            _make_move(_board, _nul_index, DIRECTION::RIGHT);
            result = true;
        }
    }

    if (result) {
        _is_solved = _check_solved(_board);
        _num_shifts++;
        _states.push({idx, swap_idx});

        return {true, QVariant(swap_idx)};
    } else {
        return {false, QVariant()};
    }
}

QPair<bool, QPair<QVariant, QVariant> > BoardModel::back_move() {
    if (!_states.empty()) {
        const auto prev = _states.pop();
        qSwap(_board[prev.first], _board[prev.second]);
        _nul_index = prev.second;
        _num_shifts--;
        _is_solved = _check_solved(_board);

        return {true, {QVariant(prev.first), QVariant(prev.second)}};
    } else {
        return {false, {QVariant(), QVariant()}};
    }
}

QPair<QVector<int>, int> BoardModel::_gen_board(int complexity) {
    auto final_state = _get_solved_board();
    int nul_index = GAME_SIZE - 1;

    for (int i = 0; i < complexity; i++) {
        _make_move(final_state, nul_index, _gen_direction());
    }

    return {final_state, nul_index};
}

bool BoardModel::_check_direction(const int nul_idx, const DIRECTION direction) {
    if (direction == DIRECTION::UP) {
        return nul_idx > _GAME_SHAPE - 1;
    } else if (direction == DIRECTION::DOWN) {
        return nul_idx < GAME_SIZE - _GAME_SHAPE;
    } else if (direction == DIRECTION::LEFT) {
        return  nul_idx % _GAME_SHAPE != 0;
    } else if (direction == DIRECTION::RIGHT) {
        return nul_idx % _GAME_SHAPE != _GAME_SHAPE - 1;
    } else {
        return false;
    }
}

bool BoardModel::_check_is_neighbour(const int lhs, const int rhs, const DIRECTION direction) {
    bool result = false;
    switch (direction) {

        case DIRECTION::UP:

             result =  lhs - _GAME_SHAPE == rhs;
             if (!result) {
                 result = rhs - _GAME_SHAPE == lhs;
             }
             break;

        case DIRECTION::DOWN:

            result = lhs + _GAME_SHAPE == rhs;
            if (!result) {
                result = rhs + _GAME_SHAPE == lhs;
            }
            break;

        case DIRECTION::LEFT:

            result = lhs - 1 == rhs;
            if (!result) {
                result = rhs - 1 == lhs;
            }
            break;

        case DIRECTION::RIGHT:

            result = lhs + 1 == rhs;
            if (!result) {
                result = rhs + 1 == lhs;
            }
            break;
    }

    return result;
}

bool BoardModel::_make_move(QVector<int> &board, int& nul_idx, const DIRECTION direction) {
    if (_check_direction(nul_idx, direction)) {
        if (direction == DIRECTION::UP) {
            qSwap(board[nul_idx], board[nul_idx - _GAME_SHAPE]);
            nul_idx -= _GAME_SHAPE;
        } else if (direction == DIRECTION::DOWN) {
            qSwap(board[nul_idx], board[nul_idx + _GAME_SHAPE]);
            nul_idx += _GAME_SHAPE;
        } else if (direction == DIRECTION::LEFT) {
            qSwap(board[nul_idx], board[nul_idx - 1]);
            nul_idx--;
        } else if (direction == DIRECTION::RIGHT) {
            qSwap(board[nul_idx], board[nul_idx + 1]);
            nul_idx++;
        }

        return true;
    } else {
        return false;
    }
}

BoardModel::DIRECTION BoardModel::_gen_direction() {
    auto gen = QRandomGenerator::global();
    switch (gen->bounded(1, 5)) {
        case 1:
            return DIRECTION::UP;
        case 2:
            return DIRECTION::DOWN;
        case 3:
            return DIRECTION::LEFT;
        case 4:
            return DIRECTION::RIGHT;
    }
}

QVector<int> BoardModel::_get_solved_board() {
    QVector<int> final_state(GAME_SIZE);
    for (int i = 0; i < GAME_SIZE; i++) {
        final_state[i] = i + 1;
    }
    final_state[GAME_SIZE - 1] = _EMPTY_ELEMENT;

    return final_state;
}

bool BoardModel::_check_solved(const QVector<int>& board) {
    int num_invs = 0;

    for (int i = 0; i < GAME_SIZE; i++) {
        if (board[i] != (i + 1) % GAME_SIZE) {
            num_invs++;
        }
    }

    return num_invs == 0;
}
