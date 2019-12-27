#include "rect.h"
#include <vector>
#include <algorithm>

void Rect::setRect (const Rect& r) {
    _left_x = r.getLx();
    _bottom_y = r.getBy();
    _right_x = r.getRx();
    _top_y = r.getTy();
}

void Rect::shift (double offset_x, double offset_y) {
    _left_x   += offset_x;
    _right_x  += offset_x;
    _bottom_y += offset_y;
    _top_y    += offset_y;
}

void Rect::absorb (const Rect& r) {
    if (_left_x > r.getLx()) {
        _left_x = r.getLx();
    }
    if (_bottom_y > r.getBy()) {
        _bottom_y = r.getBy();
    }
    if (_right_x < r.getRx()) {
        _right_x = r.getRx();
    }
    if (_top_y < r.getTy()) {
        _top_y = r.getTy();
    }
}

void Rect::orderCorrect () {
    if (_left_x > _right_x) {
        double tmp = _left_x;
        _left_x = _right_x;
        _right_x = tmp;
    }
    if (_bottom_y > _top_y) {
        double tmp = _bottom_y;
        _bottom_y = _top_y;
        _top_y = tmp;
    }
}

Rect Rect::getOverlapRectLoose(const Rect& r) {
    std::vector<double> x, y;
    x.emplace_back(_left_x);
    x.emplace_back(_right_x);
    x.emplace_back(r.getLx());
    x.emplace_back(r.getRx());
    y.emplace_back(_bottom_y);
    y.emplace_back(_top_y);
    y.emplace_back(r.getBy());
    y.emplace_back(r.getTy());
    std::sort(x.begin(), x.end());
    std::sort(y.begin(), y.end());
    return Rect(x[1], y[1], x[2], y[2]);
}

bool Rect::isCoveredInBoundary (const Rect& r) const {
    if (r.getLx() == r.getRx()) { // vertical
        if (r.getCx() == _left_x || r.getCx() == _right_x) {
            if (r.getBy() >= _bottom_y && r.getTy() <= _top_y) {
                return true;
            }
        }
    } else if (r.getBy() == r.getTy()) {
        if (r.getCy() == _bottom_y || r.getCy() == _top_y) {
            if (r.getLx() >= _left_x && r.getRx() <= _right_x) {
                return true;
            }
        }
    }
    return false;
}

bool Rect::isOverlapRect (const Rect& r) const {
    bool x_not_overlap = (_right_x < r.getLx() || _left_x > r.getRx());
    bool y_not_overlap = (_top_y < r.getBy() || _bottom_y > r.getTy());
    return !(x_not_overlap || y_not_overlap);
}

bool Rect::isStrictOverlapRect (const Rect& r) const {
    bool x_not_overlap = (_right_x <= r.getLx() || _left_x >= r.getRx());
    bool y_not_overlap = (_top_y <= r.getBy() || _bottom_y >= r.getTy());
    return !(x_not_overlap || y_not_overlap);
}
