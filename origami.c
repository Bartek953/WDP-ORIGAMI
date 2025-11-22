#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

//i will be treating vectors as points and points as vectors

double get_epsilon(void){
    return 0.000000001; //10^-9
}
double absolute(double x){
    if(x < 0)
        return -x;
    return x;
}

//struct of 2-dim vector
typedef struct vector {
    double x;
    double y;
} vector;

vector create_vector(double x, double y){
    return (vector){x, y};
}

void print_vector(vector v, char end){
    printf("(%lf, %lf)%c", v.x, v.y, end);
}

vector add_vectors(vector a, vector b){
    return (vector){a.x + b.x, a.y + b.y};
}

vector substract_vectors(vector a, vector b){
    return add_vectors(a, (vector){-b.x, -b.y});
}

vector mult_vector(double k, vector v){
    return (vector){k * v.x, k * v.y};
}

double vector_length(vector v){
    return sqrt(v.x * v.x + v.y * v.y);
}

bool equal_vectors(vector a, vector b){
    double diff = vector_length(substract_vectors(a, b));
    return diff < get_epsilon();
}

vector perpendicular_vector(vector a){
    return (vector){-a.y, a.x};
}

double vector_product(vector a, vector b){
    return a.x * b.y - a.y * b.x;
}

typedef struct line {
    vector from;
    vector to;
} line;

line create_line(vector from, vector to){
    assert(!equal_vectors(from, to));
    return (line){from, to};
}

vector symmetry(vector point, line sym_line){
    assert(!equal_vectors(sym_line.from, sym_line.to));
    //I will use some facts from GAL I lectures
    //1. Im translating vector space and creating base of space:
    vector base1 = substract_vectors(sym_line.to, sym_line.from);
    vector base2 = perpendicular_vector(base1);
    vector moved_point = substract_vectors(point, sym_line.from);

    //now I would like to represent moved_point as linear combination of base1, base2
    //so lets solve moved_point = A * base1 + B * base2
    //Krammer method:
    const double W = base1.x * base2.y - base1.y * base2.x;
    const double WA = moved_point.x * base2.y - moved_point.y * base2.x;
    const double WB = base1.x * moved_point.y - base1.y * moved_point.x;
    assert(W != 0);
    
    const double A = WA / W;
    const double B = WB / W;

    //symmetry: if V = W1 + W2 (direct sum) and v = w1 + w2, then symmetry f is defined as:
    //f(v) = w1 - w2 (w1 \in W1, w2 \in W2) (W1, W2, V are vector spaces)
    //now from definition of symmetry with respect to base1, along base2:
    vector sym_point = substract_vectors(mult_vector(A, base1), mult_vector(B, base2));

    //last thing: we need to reverse translation
    return add_vectors(sym_point, sym_line.from);
}

//location of point compared to symmetry line
typedef enum point_location {
    LEFT, MIDDLE, RIGHT
} point_location;


point_location get_point_location(vector point, line sym_line){
    vector point_vector = substract_vectors(point, sym_line.from);
    vector line_vector = substract_vectors(sym_line.to, sym_line.from);

    double vec_product = vector_product(point_vector, line_vector);

    if(absolute(vec_product) < get_epsilon())
        return MIDDLE;
    if(vec_product < 0)
        return LEFT;
    else
        return RIGHT;
}



int main(void){


    return 0;
}
