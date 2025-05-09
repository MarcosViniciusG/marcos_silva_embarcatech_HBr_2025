#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_BALLS 5
#define N 5
#define MAX_PEGS 15 // ((1+N) * N) / 2
#define H 64
#define W 128

const double PI = 3.1415926535;
const double GRAVITY = 0.75;
const int HORIZONTAL_SPACE = 4;
const int VERTICAL_SPACE = 2;
const int START_COLUMN=W/2;
const int START_ROW=5;

typedef struct Ball {
    int x, y, radius;
    int vx, vy;
} Ball;

typedef struct Peg {
    int x, y, radius;
} Peg;

typedef struct Experiment {
    int n_pegs;
    int n_balls;

    int matrix[H][W];

    Peg pegs[MAX_PEGS];
    Ball balls[MAX_BALLS];

    int result[N+1];
} Experiment;

void update_display(Experiment *experiment);

void setup_ball(Experiment *experiment, int x, int y, int radius) {
    Ball ball = {x, y, radius, 0, 0};
    experiment->balls[experiment->n_balls] = ball;
    experiment->n_balls++;
}

void setup_peg(Experiment *experiment, int x, int y, int radius) {
    Peg peg = {x, y, radius};
    experiment->pegs[experiment->n_pegs] = peg;
    experiment->n_pegs++;    
}

void setup_experiment(Experiment *experiment) {
    experiment->n_balls=0;
    experiment->n_pegs=0;

    // Initialize every peg
    for(int row=0; row<N; row++) {
        for(int column=0; column <= row; column++) {
            setup_peg(experiment, (column+START_COLUMN-(row*HORIZONTAL_SPACE/2)) + column*HORIZONTAL_SPACE, START_ROW+row*VERTICAL_SPACE, 2);
        }
    }

    update_display(experiment);
}

// Display 
void draw_circle(Experiment *experiment, int cy, int cx, int radius) {
    experiment->matrix[cy][cx] = 1;
}

void update_display(Experiment *experiment) {
    for(int i=0; i<H; i++) {
        for(int j=0; j<W; j++)
            experiment->matrix[i][j] = 0;
    }

    for(int i=0; i<experiment->n_balls; i++) {
        draw_circle(experiment, experiment->balls[i].y, experiment->balls[i].x, experiment->balls[i].radius);
    } 
    
    for(int i=0; i<experiment->n_pegs; i++) {
        draw_circle(experiment, experiment->pegs[i].y, experiment->pegs[i].x, experiment->pegs[i].radius);
    } 
}

int main() {
    srand(time(NULL)); 
    int r = rand(); 

    Experiment experiment;
    setup_experiment(&experiment);
    for(int i=0; i<H; i++) {
        for(int j=0; j<W; j++) {
            printf("%c", experiment.matrix[i][j] ? '*' : '.');
        }
        printf("\n");
    }


    return 0;
}