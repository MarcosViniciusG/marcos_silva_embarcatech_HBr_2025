#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "math.h"
#include "stdlib.h"     
#include "time.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

#define MAX_BALLS 10
#define N 5
#define MAX_PEGS 15 // ((1+N) * N) / 2
#define H 64
#define W 128

const double PI = 3.1415926535;
const double GRAVITY = 1.2;
const int HORIZONTAL_SPACE = 20;
const int VERTICAL_SPACE = 7;
const int START_COLUMN=W/2 - 1;
const int START_ROW=10;
const double DT = 0.35;
const int BALL_RADIUS=1;
const int PEG_RADIUS=2;
const int STOP_BALLS = 10000;
static int BALLS=0;

typedef struct Ball {
    int x, y, radius;
    double vx, vy;
} Ball;

typedef struct Peg {
    int x, y, radius;
} Peg;

typedef struct Experiment {
    int n_pegs;
    int n_balls;

    Peg pegs[MAX_PEGS];
    Ball balls[MAX_BALLS];

    int result[N+1];
} Experiment;

// Global experiment variable
static Experiment experiment;

int rand_mod(int mod) {
    int r = rand() % mod;

    return r;
}

int intceil(int a, int b) {
    return (a / b) + (a % b != 0);
}

void setup_ball( int x, int y, int radius) {
    int vx = (rand_mod(2) ? 1: -1) * rand_mod(3);
    if(vx==0)
        vx=rand_mod(2) ? 1: -1;

    int x_offset = rand_mod(2) ? 1 : -1;
    Ball ball = {x + x_offset, y, radius, vx, 0};
    experiment.balls[experiment.n_balls] = ball;
    experiment.n_balls++;
}

void setup_peg( int x, int y, int radius) {
    Peg peg = {x, y, radius};
    experiment.pegs[experiment.n_pegs] = peg;
    experiment.n_pegs++;    
}

void setup_experiment() {
    experiment.n_balls=0;
    experiment.n_pegs=0;
    for(int i=0; i<=N; i++)
        experiment.result[i] = 0;

    // Initialize every peg
    for(int row=0; row<N; row++) {
        for(int column=0; column <= row; column++) {
            setup_peg((column+START_COLUMN-(intceil(row*HORIZONTAL_SPACE, 2))) + column*HORIZONTAL_SPACE, START_ROW+row*VERTICAL_SPACE, PEG_RADIUS);
        }
    }
}

void collision_handler() {
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
    // Update every ball currently in the experiment
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

void driver_i2c_init() {
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void display_init(ssd1306_t *disp) {
    driver_i2c_init();

    disp->external_vcc=false;
    ssd1306_init(disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(disp);
}

void draw_circle(ssd1306_t *disp, int cy, int cx, int radius) {
    /*
        Draws the circle on the matrix
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
    ssd1306_clear(disp);

    for(int i=0; i<experiment.n_balls; i++) {
        draw_circle(disp, experiment.balls[i].y, experiment.balls[i].x, experiment.balls[i].radius);
    } 
    
    for(int i=0; i<experiment.n_pegs; i++) {
        draw_circle(disp, experiment.pegs[i].y, experiment.pegs[i].x, experiment.pegs[i].radius);
    } 
    ssd1306_show(disp);
}

int main()
{
    stdio_init_all();

    ssd1306_t disp;
    display_init(&disp);

    srand(time(NULL));
    setup_experiment();

    while (true) {
        update_display(&disp);
        if(experiment.n_balls < MAX_BALLS)
            setup_ball(START_COLUMN, 1, BALL_RADIUS);

        update_balls();
    }
}
