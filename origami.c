#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

//this function is just wrapper for error-checking (for malloc and scanf)
void catch_error(bool x, char type[], int line){
    if(!x){
        printf("[%s ERROR] At line %d\n", type, line);
        perror("Error\n");
        exit(EXIT_FAILURE);
    }
}

//the basic idea about this solution is that instead of folding figure with respect to fold line
//we can copy the point along symmetry line (fold line) and recursively continue this process
//(ofcourse if point is on the right of the line, we destroy it instead of copyig)
//this gives us time complexity of O(q * 2^n) and memmory complexity of O(n)
//(it's dfs on binary tree)

//I will be treating vectors as points and points as vectors
//and I'll use some basic linear algebra for calculating symmetries, perpendicular vectors etc.

#define EPSILON 0.000001
inline double absolute(double x){
    if(x < 0)
        return -x;
    return x;
}

//struct of 2-dim vector
typedef struct vector {
    double x;
    double y;
} vector;

inline vector create_vector(double x, double y){
    return (vector){x, y};
}

//some basic operations on vectors
inline vector add_vectors(vector a, vector b){
    return (vector){a.x + b.x, a.y + b.y};
}

inline vector subtract_vectors(vector a, vector b){
    return add_vectors(a, (vector){-b.x, -b.y});
}

inline vector mult_vector(double k, vector v){
    return (vector){k * v.x, k * v.y};
}

//(length of vector)^2
inline double vector_len2(vector v){
    return v.x * v.x + v.y * v.y;
}

inline bool equal_vectors(vector a, vector b){
    double diff = vector_len2(subtract_vectors(a, b));
    return diff < EPSILON;
}

inline vector perpendicular_vector(vector a){
    return (vector){-a.y, a.x};
}

inline double vector_product(vector a, vector b){
    return a.x * b.y - a.y * b.x;
}

typedef struct line {
    vector from;
    vector to;
} line;

inline line create_line(double x1, double y1, double x2, double y2){
    vector from = create_vector(x1, y1);
    vector to = create_vector(x2, y2);
    assert(!equal_vectors(from, to));
    return (line){from, to};
}

vector symmetry(vector point, line sym_line){
    assert(!equal_vectors(sym_line.from, sym_line.to));
    //I will use some facts from GAL I lectures
    //1. Im translating vector space and creating base of space:
    vector base1 = subtract_vectors(sym_line.to, sym_line.from);
    vector base2 = perpendicular_vector(base1);
    vector moved_point = subtract_vectors(point, sym_line.from);

    //now I would like to represent moved_point as linear combination of base1, base2
    //so lets solve moved_point = A * base1 + B * base2
    //Crammer method:
    const double W = base1.x * base2.y - base1.y * base2.x;
    const double WA = moved_point.x * base2.y - moved_point.y * base2.x;
    const double WB = base1.x * moved_point.y - base1.y * moved_point.x;

    assert(absolute(W) >= EPSILON);
    //W mathematically cannot be zero because base1 and base2 are linearly independent (it's base)
    const double A = WA / W;
    const double B = WB / W;

    //symmetry: if V = W1 + W2 (direct sum) and v = w1 + w2, then symmetry f is defined as:
    //f(v) = w1 - w2 (w1 \in W1, w2 \in W2) (W1, W2, V are vector spaces)
    //now from definition of symmetry with respect to base1, along base2:
    vector sym_point = subtract_vectors(mult_vector(A, base1), mult_vector(B, base2));

    //last thing: we need to reverse translation
    return add_vectors(sym_point, sym_line.from);
}

//location of point compared to symmetry line
typedef enum {
    LEFT, MIDDLE, RIGHT
} point_location;

//checks on which side of the symmetry/fold line is the point
point_location get_point_location(vector point, line sym_line){
    vector point_vector = subtract_vectors(point, sym_line.from);
    vector line_vector = subtract_vectors(sym_line.to, sym_line.from);

    double vec_product = vector_product(point_vector, line_vector);

    if(absolute(vec_product) < EPSILON)
        return MIDDLE;
    if(vec_product < 0)
        return LEFT;
    else
        return RIGHT;
}

typedef struct fold {
    line sym_line;
    struct fold* prev;
} fold;

typedef enum {
    EMPTY, RECTANGLE, CIRCLE
} shape;

typedef struct figure {
    shape type;
    vector point1;
    vector point2;
    double r;
    fold* top_fold;
} figure;

figure create_figure(shape type, vector p1, vector p2, double r){
    //assert(type == CIRCLE || !equal_vectors(p1, p2));
    return (figure){
        .type = type,
        .point1 = p1,
        .point2 = p2,
        .r = r,
        .top_fold = NULL
    };
}

//just adds new fold to existing figure
void add_fold(figure* origami, double x1, double y1, double x2, double y2){
    assert(!equal_vectors((vector){x1, y1}, (vector){x2, y2}));
    assert(origami != NULL);

    fold* new_fold = (fold*) malloc(sizeof(fold));
    catch_error(new_fold, "MALLOC", __LINE__);

    (*new_fold).sym_line = create_line(x1, y1, x2, y2);
    (*new_fold).prev = origami->top_fold;

    origami->top_fold = new_fold;
}

//checks whenever point is in object
bool in_figure(vector point, figure* object){
    if(object->type == CIRCLE 
    && vector_len2(subtract_vectors(point, object->point1)) <= object->r * object->r + EPSILON){
        return true;
    }
    if(object->type == RECTANGLE 
    && object->point1.x <= point.x  + EPSILON 
    && point.x <= object->point2.x  + EPSILON
    && object->point1.y <= point.y  + EPSILON 
    && point.y <= object->point2.y  + EPSILON){
        return true;
    }
    return false;
}

//this function does the main logic - for each fold it doubles the point or destroys it
//(depends whenever the point is on the right or left of line)
//and recurently calculates next fold
//if it gets figure without fold - it checks if the point is in that figure and if yes, increments result
void calc_res(figure* origami, fold* curr_fold, vector point, int* result){
    if(curr_fold == NULL){
        if(in_figure(point, origami)){
            (*result)++;
        }
    }
    else {
        vector sym_point = symmetry(point, curr_fold->sym_line);

        point_location location = get_point_location(point, curr_fold->sym_line);

        if(location == LEFT){
            calc_res(origami, curr_fold->prev, point, result);
            calc_res(origami, curr_fold->prev, sym_point, result);
        }
        if(location == MIDDLE){
            calc_res(origami, curr_fold->prev, point, result);
        }
        //if RIGHT, then we destroy the point
    }
}

int main(void){
    int n, q;
    catch_error(scanf("%d %d", &n, &q) == 2, "SCANF", __LINE__);

    figure* origamis = (figure*) malloc((size_t) (n + 1) * sizeof(figure));
    catch_error(origamis, "MALLOC", __LINE__);


    char c;
    int k;
    double x1, y1, x2, y2, r;
    for(int i = 1; i <= n; i++){
        catch_error(scanf(" %c", &c) == 1, "SCANF", __LINE__);
        switch(c){
            case 'P': {
                catch_error(scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2) == 4, "SCANF", __LINE__);
                vector p1 = create_vector(x1, y1);
                vector p2 = create_vector(x2, y2);
                origamis[i] = create_figure(RECTANGLE, p1, p2, -1);
                break;
            }
            case 'K': {
                catch_error(scanf("%lf %lf %lf", &x1, &y1, &r) == 3, "SCANF", __LINE__);
                vector p1 = create_vector(x1, y1);
                origamis[i] = create_figure(CIRCLE, p1, p1, r);
                break;
            }
            case 'Z': {
                catch_error(scanf("%d %lf %lf %lf %lf", &k, &x1, &y1, &x2, &y2) == 5, "SCANF", __LINE__);
                assert(1 <= k && k < i);
                origamis[i] = origamis[k];
                add_fold(&origamis[i], x1, y1, x2, y2);
                break;
            }
            default: {
                printf("[ERROR] Could recognize type of %c at %d-th line\n", c, i);
                exit(-1);
            }
        }
    }

    for(int i = 1; i <= q; i++){
        catch_error(scanf("%d %lf %lf", &k, &x1, &y1) == 3, "SCANF", __LINE__);
        assert(1 <= k && k <= n);
        int result = 0;
        calc_res(&origamis[k], origamis[k].top_fold, (vector){x1, y1}, &result);
        printf("%d\n", result);
    }

    //every figure could create up to 1 fold - itd top fold
    //and every fold is created by exacly one figure
    //therefore every figure needs to clean only after itself
    for(int i = 1; i <= n; i++){
        if(origamis[i].top_fold != NULL){
            free(origamis[i].top_fold);
        }
    }
    free(origamis);
    return 0;
}
