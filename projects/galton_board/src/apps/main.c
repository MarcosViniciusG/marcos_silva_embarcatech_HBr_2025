#include <stdio.h>
#include "pico/stdlib.h"
#include "aux.h"
#include "math.h"
#include "stdlib.h"     
#include "time.h"
#include "ssd1306_i2c.h"

// --- Macros ---
#define MAX_BALLS 10
#define N 5                      // Number of rows
#define MAX_PEGS 15              // ((1+N) * N) / 2
#define H 64                     // Height of the display
#define W 128                    // Width of the display

// --- Structs ---
typedef struct Ball {         
    /*
        Ball struct with
        coordinates, radius and velocity
    */
    int x, y, radius;
    double vx, vy;
} Ball;

typedef struct Peg {            
    /*
        Peg struct with
        coordinates and radius
    */
    int x, y, radius;
} Peg;

typedef struct Experiment {
    /*
        Experiment struct responsible
        for keeping track of every ball, peg
        and the final result
    */

    int n_pegs;
    int n_balls;

    Peg pegs[MAX_PEGS];
    Ball balls[MAX_BALLS];

    int result[N+1];
} Experiment;

// --- Global constants ---
const double GRAVITY = 1.2;      // Gravity in pixels / second^2
const int HORIZONTAL_SPACE = 20; // Horizontal spacing between every peg
const int VERTICAL_SPACE = 7;    // Vertical spacing between every peg
const int START_COLUMN=W/2 - 1;  // Horizontal starting position of the ball/peg
const int START_ROW=10;          // Vertical starting position of the ball/peg
const double DT = 0.35;          // Time passed since last clock
const int BALL_RADIUS=1;         // Radius of the ball
const int PEG_RADIUS=2;          // Radius of the pegs
const int STOP_BALLS = 10000;    // Experiment will stop

// --- Global variables ---
static int BALLS=0;              // Number of balls that reached the end
static Experiment experiment;    // Global experiment variable (Singleton)

// --- Prototypes ---
void setup_ball(int x, int y, int radius);
void setup_peg(int x, int y, int radius);
void setup_experiment();
void collision_handler();
void update_balls();
void draw_circle(ssd1306_t *disp, int cy, int cx, int radius);
void update_display(ssd1306_t *disp);

void setup_ball(int x, int y, int radius) {
    /*
        Initializes ball with center in x and y
        and r = radius
        Random x velocity
    */

    int vx = (rand_mod(2) ? 1: -1) * rand_mod(3);
    if(vx==0)
        vx=rand_mod(2) ? 1: -1;

    int x_offset = rand_mod(2) ? 1 : -1;
    Ball ball = {x + x_offset, y, radius, vx, 0};
    experiment.balls[experiment.n_balls] = ball;
    experiment.n_balls++;
}

void setup_peg(int x, int y, int radius) {
    /*
        Initializes peg with center in x and y
    */

    Peg peg = {x, y, radius};
    experiment.pegs[experiment.n_pegs] = peg;
    experiment.n_pegs++;    
}

void setup_experiment() {
    /*
        Starts the experiment. Initializes
        every variable with zero and puts
        the pegs in their places
    */

    experiment.n_balls=0;
    experiment.n_pegs=0;
    for(int i=0; i<=N; i++)
        experiment.result[i] = 0;

    // Initializes every peg
    for(int row=0; row<N; row++) {
        for(int column=0; column <= row; column++) {
            setup_peg((column+START_COLUMN-(intceil(row*HORIZONTAL_SPACE, 2))) + column*HORIZONTAL_SPACE, START_ROW+row*VERTICAL_SPACE, PEG_RADIUS);
        }
    }
}

void collision_handler() {
    /*
        Handles collision between balls
        and pegs. Uses brute force (check every pair of ball
        and peg).
    */
    for(int i=0; i<experiment.n_balls; i++) {
        Ball *ball = &experiment.balls[i];
        
        // Check for every peg if the ball has collided with them
        for(int j=0; j<experiment.n_pegs; j++) {
            Peg *peg = &experiment.pegs[j];

            int dx = ball->x - peg->x;
            int dy = ball->y - peg->y;

            int distance = dx*dx + dy*dy;
            int r_sum = ball->radius + peg->radius;
            

            double normal_x = dx / sqrt(distance);
            double normal_y = dy / sqrt(distance);

            if(distance <= r_sum*r_sum) {
                double dot = -2*((normal_x * ball->vx) + (normal_y * ball->vy));
                if(dot > 0) {
                    ball->vx = ball->vx + (normal_x * dot);
                    ball->vy = ball->vy + (normal_y * dot) * 0.5;
                }
            }
        }

        // Check if the ball has collided with the walls
        if(ball->x >= W-1 || ball->x <= 0) {
            ball->vx = -ball->vx;
            if(ball->x >= W-1)
                ball->x = W-2;
            else
                ball->x = 1;
        }
        if(ball->y <= 0) {
            ball->vy = -ball->vy;
            ball->y = 1;
        }
        if(ball->y >= H-1) {
            int local = ball->x / (W / (N+1));
            for (int k = i; k < experiment.n_balls - 1; k++) {
                experiment.balls[k] = experiment.balls[k + 1];
            }
            i--;
            experiment.n_balls--;

            experiment.result[local]++;
            BALLS++;
        }
    }
}

void update_balls() {
    /*
        Updates the velocity and position of
        every ball currently in the experiment.
        Checks for collision after updating the ball.
    */
    for(int i=0; i<experiment.n_balls; i++) {
        Ball *ball = &experiment.balls[i];
        
        // Update velocity of the balls
        ball->vy += GRAVITY*DT;
        
        // Update positions of the balls
        ball->y += ball->vy*DT;

        double new_x = ceil(ball->vx*DT); 
        if(new_x < 0)
            new_x = floor(ball->vx*DT);
        else if(new_x > 0)
            new_x = ceil(ball->vx*DT);
        else
            new_x = rand_mod(2) ? 1 : -1;


        ball->x += new_x;
        collision_handler(experiment);
    }
}

void draw_circle(ssd1306_t *disp, int cy, int cx, int radius) {
    /*
        Draws the circle on the matrix
        with center in cy and cx and radius = radius
    */

    ssd1306_draw_pixel(disp, cx, cy);
    for (int y = -radius; y <= radius; y++) {
        int y_pos = cy + y;
        if (y_pos < 0 || y_pos >= H) continue;

        int dx = (int)sqrt(radius * radius - y * y);
        int x_start = cx - dx;
        int x_end = cx + dx;

        if (x_start < 0) x_start = 0;
        if (x_end >= W) x_end = W - 1;

        for (int x = x_start; x <= x_end; x++) {
            ssd1306_draw_pixel(disp, x, y_pos);
        }
    }
}

void update_display(ssd1306_t *disp) {
    /*
        Draw the entire experiment on the
        display. Balls, pegs, total number of
        balls and histogram
    */

    ssd1306_clear(disp); // Clean the display

    // Draw each ball
    for(int i=0; i<experiment.n_balls; i++) {
        draw_circle(disp, experiment.balls[i].y, experiment.balls[i].x, experiment.balls[i].radius);
    } 
    
    // Draw each peg
    for(int i=0; i<experiment.n_pegs; i++) {
        draw_circle(disp, experiment.pegs[i].y, experiment.pegs[i].x, experiment.pegs[i].radius);
    } 
    
    // Show on the display
    ssd1306_show(disp);
}

int main()
{
    /*
        Main loop of the experiment
    */
    stdio_init_all();

    ssd1306_t disp;
    display_init(&disp);

    init_rand();
    setup_experiment();

    while (true) {
        update_display(&disp);
        if(experiment.n_balls < MAX_BALLS)
            setup_ball(START_COLUMN, 1, BALL_RADIUS);

        update_balls();
    }
}