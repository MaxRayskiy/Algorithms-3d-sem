/*
 Даны два отрезка в пространстве (x1, y1, z1) - (x2, y2, z2) и (x3, y3, z3) - (x4, y4, z4).
 Найдите расстояние между отрезками.
 */
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

static double EPS = 1e-6;

struct Point {
  double x, y ,z;

  Point(double&  x, double&  y, double&  z) : x(x), y(y), z(z) {}
  Point(double&& x, double&& y, double&& z) : x(x), y(y), z(z) {}

};

struct LineSegment {
  Point FirstPoint, SecondPoint;
  LineSegment(Point& First, Point& Second) : FirstPoint(First), SecondPoint(Second) {}
};

double CalcDist(Point& first_point, Point& second_point) {
    return sqrt(pow((first_point.x - second_point.x), 2)
              + pow((first_point.y - second_point.y), 2)
              + pow((first_point.z - second_point.z), 2));
}

Point FindClosestPointOnSegment(LineSegment& Segment, Point& Dot);

double FindDist(LineSegment& FirstSegment, LineSegment& SecondSegment);

int main() {
    double x, y, z;

    std::cin >> x >> y >> z;
    Point A (x, y, z);

    std::cin >> x >> y >> z;
    Point B (x, y, z);

    std::cin >> x >> y >> z;
    Point C (x, y, z);

    std::cin >> x >> y >> z;
    Point D (x, y, z);

    LineSegment FirstSegment(A, B);
    LineSegment SecondSegment(C, D);

    std::cout << std::setprecision(10) << FindDist(FirstSegment, SecondSegment) << std::endl;
    return 0;
}

double FindDist(LineSegment& FirstSegment, LineSegment& SecondSegment) {
    //  ternary search
    Point First =  FirstSegment.FirstPoint;
    Point Second = FirstSegment.SecondPoint;

    Point Result(0,0,0);

    while(CalcDist(First, Second) > EPS) {
        //  divide FirstSegment in 3 parts by Midle1 and Midle2 :  First --- Midle1 --- Midle2 --- Second
        Point Midle1(First.x * 2 / 3 + Second.x / 3,
                     First.y * 2 / 3 + Second.y / 3,
                     First.z * 2 / 3 + Second.z / 3);

        Point Midle2(First.x * 1 / 3 + Second.x * 2 / 3,
                     First.y * 1 / 3 + Second.y * 2 / 3,
                     First.z * 1 / 3 + Second.z * 2 / 3);

        Point SecondSegmentPoint1 = FindClosestPointOnSegment(SecondSegment, Midle1);  //  closest point to Midle1
        Point SecondSegmentPoint2 = FindClosestPointOnSegment(SecondSegment, Midle2);  //  closest point to Midle2

        //  Cut one part of segment
        if (CalcDist(Midle1, SecondSegmentPoint1) < CalcDist(Midle2, SecondSegmentPoint2)) {
            FirstSegment.SecondPoint = Midle2;
            Second = Midle2;
            Result = SecondSegmentPoint1;
        } else {
            FirstSegment.FirstPoint = Midle1;
            First = Midle1;
            Result = SecondSegmentPoint2;
        }
    }

//    return CalcDist(First, Result);
    return std::min(std::min(std::min(CalcDist(FirstSegment.FirstPoint, SecondSegment.SecondPoint),
                                      CalcDist(FirstSegment.FirstPoint, SecondSegment.FirstPoint)),
                             std::min(CalcDist(FirstSegment.SecondPoint, SecondSegment.SecondPoint),
                                      CalcDist(FirstSegment.SecondPoint, SecondSegment.FirstPoint))),
                    CalcDist(First, Result));

}

Point FindClosestPointOnSegment(LineSegment& Segment, Point& Dot) {
    //  ternary search
    Point First =  Segment.FirstPoint;
    Point Second = Segment.SecondPoint;

    while (CalcDist(First, Second) > EPS) {
        //  divide Segment in 3 parts by Midle1 and Midle2 :  First --- Midle1 --- Midle2 --- Second
        Point Midle1(First.x * 2 / 3 + Second.x / 3,
                     First.y * 2 / 3 + Second.y / 3,
                     First.z * 2 / 3 + Second.z / 3);

        Point Midle2(First.x / 3 + Second.x * 2 / 3,
                     First.y / 3 + Second.y * 2 / 3,
                     First.z / 3 + Second.z * 2 / 3);


        //  Cut one part of segment
        if (CalcDist(Dot, Midle1) < CalcDist(Dot, Midle2)) {
            Second = Midle2;
        } else {
            First = Midle1;
        }
    }
    if (CalcDist(Dot, First) < CalcDist(Dot, Second)) {
        return First;
    } else {
        return Second;
    }
}
