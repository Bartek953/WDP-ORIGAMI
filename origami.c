#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

//i will be treating vectors as points and points as vectors

double get_epsilon(void){
    return 0.00000001; //10^-8
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

line create_line(double x1, double y1, double x2, double y2){
    vector from = create_vector(x1, y1);
    vector to = create_vector(x2, y2);
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

    assert(absolute(W) >= get_epsilon());
    
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

typedef struct fold {
    line sym_line;
    struct fold* prev;
} fold;

typedef enum shape {
    EMPTY, RECTANGLE, CIRCLE
} shape;

typedef struct figure {
    shape type;
    vector point1;
    vector point2;
    double r;
    fold* top_fold;
} figure;

figure create_figure(void){
    figure result;
    result.type = EMPTY;
    result.point1 = create_vector(0, 0);
    result.point2 = create_vector(-1, -1);
    result.r = -1;
    result.top_fold = NULL;
    return result;
}

figure create_rectangle(double x1, double y1, double x2, double y2){
    assert(x1 <= x2 && y1 <= y2);
    figure result;
    result.type = RECTANGLE;
    result.point1 = create_vector(x1, y1);
    result.point2 = create_vector(x2, y2);
    result.r = -1;
    result.top_fold = NULL;
    return result;
}
figure create_circle(double x, double y, double r){
    assert(r >= 0);
    figure result;
    result.type = CIRCLE;
    result.point1 = create_vector(x, y);
    result.point2 = create_vector(x, y);
    result.r = r;
    result.top_fold = NULL;
    return result;
}
void add_fold(figure* origami, double x1, double y1, double x2, double y2){
    assert(!equal_vectors((vector){x1, y1}, (vector){x2, y2}));
    assert(origami != NULL);

    fold* new_fold = (fold*) malloc(sizeof(fold));
    (*new_fold).sym_line = create_line(x1, y1, x2, y2);
    (*new_fold).prev = origami->top_fold;

    origami->top_fold = new_fold;
}

void print_figure(figure* object){
    switch(object->type){
        case EMPTY:
            printf("TYPE: EMPTY\n");
            break;
        case RECTANGLE:
            printf("TYPE: RECTANGLE\n");
            break;
        case CIRCLE:
            printf("TYPE: CIRCLE\n");
            break;
    }
    print_vector(object->point1, ' ');
    print_vector(object->point2, '\n');
    printf("radius: %lf\n", object->r);
    printf("Folds:\n");
    fold* top_fold = object->top_fold;
    while(top_fold != NULL){
        print_vector(top_fold->sym_line.from, ' ');
        print_vector(top_fold->sym_line.to, '\n');
        top_fold = top_fold->prev;
    }
    printf("\n");
}

bool in_figure(vector point, figure* object){
    //printf("odl: %lf\n", vector_length(substract_vectors(point, object->point1)));
    if(object->type == CIRCLE 
    && vector_length(substract_vectors(point, object->point1)) <= object->r + get_epsilon()){
        return true;
    }
    if(object->type == RECTANGLE 
    && object->point1.x <= point.x  + get_epsilon()  
    && point.x <= object->point2.x  + get_epsilon()
    && object->point1.y <= point.y  + get_epsilon() 
    && point.y <= object->point2.y  + get_epsilon()){
        return true;
    }
    return false;
}

void calc_res(figure* origami, fold* curr_fold, vector point, int* result){
    //print_vector(point, '\n');
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
    }
}



int main(void){
    int n, q;
    if(scanf("%d %d", &n, &q) != 2){
        printf("[SCANF ERROR]Couldn't read n, q\n");
        exit(-1);
    }

    figure* origamis = (figure*) malloc((size_t) (n + 1) * sizeof(figure));
    if(!origamis){
        printf("[MALLOC ERROR]Could allocate origamis array with size: %d\n", n);
        exit(-1);
    }


    char c;
    int k;
    double x1, y1, x2, y2, r;
    for(int i = 1; i <= n; i++){
        if(scanf(" %c", &c) != 1){
            printf("[SCANF ERROR] Couldn't read c in %d-th line\n", i);
            exit(-1);
        }
        switch(c){
            case 'P': {
                if(scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2) != 4){
                    printf("[SCANF ERROR] Couldn't read %d-th line\n", i);
                    exit(-1);
                }
                origamis[i] = create_rectangle(x1, y1, x2, y2);
                break;
            }
            case 'K': {
                if(scanf("%lf %lf %lf", &x1, &y1, &r) != 3){
                    printf("[SCANF ERROR] Couldn't read %d-th line\n", i);
                    exit(-1);
                }
                origamis[i] = create_circle(x1, y1, r);
                break;
            }
            case 'Z': {
                if(scanf("%d %lf %lf %lf %lf", &k, &x1, &y1, &x2, &y2) != 5){
                    printf("[SCANF ERROR] Couldn't read %d-th line\n", i);
                    exit(-1);
                }
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

    /*for(int i = 1; i <= n; i++){
        printf("%d:\n", i);
        print_figure(&origamis[i]);
    }*/

    for(int i = 1; i <= q; i++){
        if(scanf("%d %lf %lf", &k, &x1, &y1) != 3){
            printf("[SCANF ERROR] Couldn't read %d-th line of queastions", i);
            exit(-1);
        }
        assert(1 <= k && k <= n);
        int result = 0;
        calc_res(&origamis[k], origamis[k].top_fold, (vector){x1, y1}, &result);
        printf("%d\n", result);
    }


    //free memmory !!!
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
