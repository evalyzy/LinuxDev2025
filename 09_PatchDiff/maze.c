#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 6
#define HEIGHT 6

#define CELL_PATH 0
#define WALL 1

typedef struct {
    int x, y;
    int visited;
    int walls[4]; // N, S, E, W
} Cell;

Cell grid[HEIGHT][WIDTH];
char *output_grid;
int output_width;
int output_height;

void initialize_grid() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            grid[i][j].x = j;
            grid[i][j].y = i;
            grid[i][j].visited = 0;
            for(int k=0; k<4; k++) grid[i][j].walls[k] = 1;
        }
    }
}

void generate_maze(int start_x, int start_y) {
    Cell* stack[WIDTH * HEIGHT];
    int stack_top = 0;

    grid[start_y][start_x].visited = 1;
    stack[stack_top++] = &grid[start_y][start_x];

    while (stack_top > 0) {
        Cell* current = stack[--stack_top];
        int x = current->x;
        int y = current->y;
        int neighbors[4][2];
        int num_neighbors = 0;

        // North
        if (y > 0 && !grid[y - 1][x].visited) {
            neighbors[num_neighbors][0] = x;
            neighbors[num_neighbors++][1] = y - 1;
        }
        // South
        if (y < HEIGHT - 1 && !grid[y + 1][x].visited) {
            neighbors[num_neighbors][0] = x;
            neighbors[num_neighbors++][1] = y + 1;
        }
        // East
        if (x < WIDTH - 1 && !grid[y][x + 1].visited) {
            neighbors[num_neighbors][0] = x + 1;
            neighbors[num_neighbors++][1] = y;
        }
        // West
        if (x > 0 && !grid[y][x - 1].visited) {
            neighbors[num_neighbors][0] = x - 1;
            neighbors[num_neighbors++][1] = y;
        }

        if (num_neighbors > 0) {
            stack[stack_top++] = current;
            int r = rand() % num_neighbors;
            int next_x = neighbors[r][0];
            int next_y = neighbors[r][1];
            Cell* next = &grid[next_y][next_x];

            if (next_x == x && next_y == y - 1) { // N
                current->walls[0] = 0; next->walls[1] = 0;
            } else if (next_x == x && next_y == y + 1) { // S
                current->walls[1] = 0; next->walls[0] = 0;
            } else if (next_x == x + 1 && next_y == y) { // E
                current->walls[2] = 0; next->walls[3] = 0;
            } else { // W
                current->walls[3] = 0; next->walls[2] = 0;
            }

            next->visited = 1;
            stack[stack_top++] = next;
        }
    }
}

void print_maze() {
    output_width = WIDTH * 2 + 1;
    output_height = HEIGHT * 2 + 1;
    output_grid = malloc(output_width * output_height);
    memset(output_grid, '#', output_width * output_height);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int out_x = x * 2 + 1;
            int out_y = y * 2 + 1;
            output_grid[out_y * output_width + out_x] = '.';
            if (grid[y][x].walls[0] == 0) output_grid[(out_y - 1) * output_width + out_x] = '.';
            if (grid[y][x].walls[1] == 0) output_grid[(out_y + 1) * output_width + out_x] = '.';
            if (grid[y][x].walls[2] == 0) output_grid[out_y * output_width + (out_x + 1)] = '.';
            if (grid[y][x].walls[3] == 0) output_grid[out_y * output_width + (out_x - 1)] = '.';
        }
    }

    for (int i = 0; i < output_height; i++) {
        for (int j = 0; j < output_width; j++) {
            putchar(output_grid[i * output_width + j]);
        }
        putchar('\n');
    }
    free(output_grid);
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv; // Suppress unused warning
    srand(time(NULL));
    initialize_grid();
    generate_maze(0, 0);
    print_maze();
    return 0;
}
