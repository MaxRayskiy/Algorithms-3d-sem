#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>

struct Point {
  double x, y;
  Point(const double& x, const double& y) : x(x), y(y) {}
  Point(double&& x, double&& y) : x(x), y(y) {}
};

inline bool operator==(const Point& lhs, const Point& rhs);
inline Point operator+(const Point& lhs, const Point& rhs);
inline void operator+=(Point& lhs, const Point& rhs);

struct Vector {
  Point beg, end;
  Point delta;
  Vector(const Point& beg, const Point& end);
  Vector(Point&& beg, Point&& end);
};

Vector::Vector(const Point& beg, const Point& end)
    : beg(beg)
    , end(end)
    , delta(end.x - beg.x, end.y - beg.y)
{}

Vector::Vector(Point&& beg, Point&& end)
    : beg(beg)
    , end(end)
    , delta(end.x - beg.x, end.y - beg.y)
{}

struct Figure {
  std::vector<Point> vertices;
};

class TwoFigures {
 public:
    TwoFigures() {
        first_fg = new Figure;
        second_fg = new Figure;
    }

    ~TwoFigures() {
        delete first_fg;
        delete second_fg;
    }
    void AddPointToFirst(std::vector<Point> coord);
    void AddPointToSecond(std::vector<Point> coord);

    bool AreIntersect();

 private:
    Figure* first_fg;
    Figure* second_fg;

    [[nodiscard]] static bool IsPosMult(const Vector& lhs, const Vector& rhs);

    static Point MinPoint(Figure* figure);

    static bool vector_cmp(const Vector& first, const Vector& second);

    struct points_cmp {
      explicit points_cmp(const Point& lhs) : special(lhs) {}
      Point special;

      bool operator()(const Point& first, const Point& second) const;
    };

    std::vector<Vector> BuildResultFigure();
};


int main() {
    TwoFigures Group;
    std::vector<Point> coord;

    int n;
    std::cin >> n;
    coord.reserve(n);
    for (int i = 0; i < n; ++i) {
        double point_x, point_y;
        std::cin >> point_x >> point_y;
        coord.emplace_back(point_x, point_y);
    }
    Group.AddPointToFirst(coord);

    std::cin >> n;
    coord.clear();
    coord.reserve(n);
    for (int i = 0; i < n; ++i) {
        double point_x, point_y;
        std::cin >> point_x >> point_y;
        coord.emplace_back(point_x, point_y);
    }
    Group.AddPointToSecond(coord);

    if (Group.AreIntersect()) {
        std::cout << "YES" << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }
    return 0;
}

inline bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline Point operator+(const Point& lhs, const Point& rhs) {
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline void operator+=(Point& lhs, const Point& rhs) {
    lhs.x += rhs.x; lhs.y += rhs.y;
}

bool TwoFigures::AreIntersect() {
    if (first_fg->vertices.size() < second_fg->vertices.size()) {
        std::swap(first_fg, second_fg);
    }
    for (auto& point : second_fg->vertices) {
        point.x *= -1;
        point.y *= -1;
    }
    Point first_min_point = MinPoint(first_fg);
    Point second_min_point = MinPoint(second_fg);

    Point result_st_point(first_min_point.x + second_min_point.x,
                          first_min_point.y + second_min_point.y);

    std::sort(first_fg->vertices.begin(),
              first_fg->vertices.end(),
              points_cmp(first_min_point));

    std::sort(second_fg->vertices.begin(),
              second_fg->vertices.end(),
              points_cmp(second_min_point));

    first_fg->vertices.push_back(first_fg->vertices[0]);
    second_fg->vertices.push_back(second_fg->vertices[0]);

    std::vector<Vector> result_fg = BuildResultFigure();

    // check (0,0) in the result figure
    Point curr_point = result_st_point;
    Point zero(0, 0);
    for (const auto& vector : result_fg) {
        Vector first(curr_point, zero);
        Vector second(curr_point + vector.delta, zero);
        if (!IsPosMult(first, second)) {
            return false;
        }
        curr_point += vector.delta;
    }
    return true;
}

void TwoFigures::AddPointToFirst(std::vector<Point> coord) {
    first_fg->vertices = std::move(coord);
}

void TwoFigures::AddPointToSecond(std::vector<Point> coord) {
    second_fg->vertices = std::move(coord);
}

bool TwoFigures::IsPosMult(const Vector& lhs, const Vector& rhs) {
    return (lhs.delta.x * rhs.delta.y) - (lhs.delta.y * rhs.delta.x) >= 0;
}

bool TwoFigures::vector_cmp(const Vector& first, const Vector& second) {
    const double& delta_x1 = first.delta.x;
    const double& delta_y1 = first.delta.y;
    const double& delta_x2 = second.delta.x;
    const double& delta_y2 = second.delta.y;

    if (delta_x1 >= 0 && delta_x2 >= 0) {
        if (delta_x1 == 0 && delta_x2 == 0) {
            if ((delta_y1 >= 0 && delta_y2 >= 0) ||
                (delta_y1 < 0 && delta_y2 < 0)) {
                return true;
            } else {
                return delta_y1 < 0;
            }
        } else if (delta_x1 == 0) {
            return delta_y1 < 0;
        } else if (delta_x2 == 0) {
            return delta_y2 > 0;
        }

        double angle1 = delta_y1 / delta_x1;
        double angle2 = delta_y2 / delta_x2;

        return angle1 < angle2;
    } else if (delta_x1 < 0 && delta_x2 < 0) {
        double angle1 = - delta_y1 / delta_x1;
        double angle2 = - delta_y2 / delta_x2;

        return angle1 > angle2;
    } else {
        return delta_x1 > 0;
    }
}

Point TwoFigures::MinPoint(Figure* figure)  {
    Point min = figure->vertices[0];
    for (auto& point : figure->vertices) {
        if (point.x < min.x || (point.x == min.x && point.y < min.y)) {
            min = point;
        }
    }
    return min;
}

std::vector<Vector> TwoFigures::BuildResultFigure() {
    std::vector<Vector> result_fg;

    int i = 1, j = 1;
    while (i < first_fg->vertices.size() && j < second_fg->vertices.size()) {
        if (vector_cmp(Vector(first_fg->vertices[i - 1],
                              first_fg->vertices[i]),
                       Vector(second_fg->vertices[j - 1],
                              second_fg->vertices[j]))) {
            result_fg.emplace_back(first_fg->vertices[i - 1],
                                   first_fg->vertices[i]);
            ++i;
        } else {
            result_fg.emplace_back(second_fg->vertices[j - 1],
                                   second_fg->vertices[j]);
            ++j;
        }
    }

    if (i == first_fg->vertices.size()) {
        while (j < second_fg->vertices.size()) {
            result_fg.emplace_back(second_fg->vertices[j - 1],
                                   second_fg->vertices[j]);
            ++j;
        }
    } else {
        while (i < first_fg->vertices.size()) {
            result_fg.emplace_back(first_fg->vertices[i - 1],
                                   first_fg->vertices[i]);
            ++i;
        }
    }
    return result_fg;
}

bool TwoFigures::points_cmp::operator()(const Point& first,
                                        const Point& second) const {
    if (first == special) {
        return true;
    } else if (second == special) {
        return false;
    }

    if (first.x == special.x && second.x == special.x) {
        return first.y > second.y;
    } else if (first.x == special.x) {
        return false;
    } else if (second.x == special.x) {
        return true;
    }
    double angle1 = (first.y - special.y) / (first.x - special.x);
    double angle2 = (second.y - special.y) / (second.x - special.x);
    return angle1 < angle2;
}
