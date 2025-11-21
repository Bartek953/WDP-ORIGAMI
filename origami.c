#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct point {
    double x;
    double y;
} point;

point create_point(double x, double y){
    return (point){.x = x, .y = y};
}

void print_point(point p, char end){
    printf("(%lf, %lf)%c", p.x, p.y, end);
}

//straight line going through points p1, p2
//should be p1 != p2
typedef struct line {
    point p1;
    point p2;
} line;

//rectangle defined by left down corner p1 and right upper corner p2
//should be: p1.x <= p2.x
//and p1.y <= p2.y
typedef struct rectangle {
    point p1;
    point p2;
} rectangle;

//circle with middle in point p1 and radius r
//should be: r > 0
typedef struct circle {
    point p1;
    double r;
} circle;

//returns point symmetrical to point1 along line1
point symmetry(point point1, line line1){
    assert(line1.p1.x != line1.p2.x || line1.p1.y != line1.p2.y);
    point result = create_point(0, 0);

    if(line1.p1.x == line1.p2.x){
        //vertical line
        result.y = point1.y;
        result.x = 2 * line1.p1.x - point1.x;
        return result;
    }
    if(line1.p1.y == line1.p2.y){
        //horizontal line
        result.x = point1.x;
        result.y = 2 * line1.p1.y - point1.y;
        return result;
    }
    //now we are not dealing with vertical or horizontal line,
    //so I will represent line1 as y = A * x + B
    //using basic analytical geometry we can solve for A, B
    const double A = (line1.p1.y - line1.p2.y) / (line1.p1.x - line1.p2.x);
    const double B = line1.p1.y - A * line1.p1.x;
    //now I will find perpendicular to y line y2 = A2 * x + B going through point1
    const double A2 = -1 / A; //from basic geometry
    const double B2 = point1.y - A2 * point1.x;
    //now i will find intersection of lines y1, y2
    point intersect_point = create_point(0, 0);
    intersect_point.x = (B2 - B) / (A - A2);
    intersect_point.y = A * intersect_point.x + B;

    printf("\n");
    print_point((point){A, B}, '\n');
    print_point((point){A2, B2}, '\n');

    //from vectors:
    result.x = point1.x + 2 * (intersect_point.x - point1.x);
    result.y = point1.y + 2 * (intersect_point.y - point1.y);

    return result;
}




int main(void){

    point p1;
    line line1;

    scanf("%lf %lf", &p1.x, &p1.y);
    scanf("%lf %lf %lf %lf", &line1.p1.x, &line1.p1.y, &line1.p2.x, &line1.p2.y);

    print_point(symmetry(p1, line1), '\n');

    return 0;
}
