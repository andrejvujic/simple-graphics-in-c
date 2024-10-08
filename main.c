#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

#define COLOR_BLACK 0xff181818
#define COLOR_RED 0xffff0000
#define COLOR_GREEN 0xff00ff00
#define COLOR_BLUE 0xff0000ff

#define FILE_READ "r"
#define FILE_WRITE "w"

typedef struct
{
    unsigned alpha, red, green, blue;
} COMPONENTS;

COMPONENTS extract_components(unsigned color)
{
    // color -> 0xaarrggbb
    COMPONENTS components;
    components.alpha = (color >> 3 * 8) & 0xff;
    components.red = (color >> 2 * 8) & 0xff;
    components.green = (color >> 1 * 8) & 0xff;
    components.blue = color & 0xff;
    return components;
}

int write_ppm(char *f_path, unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH], unsigned canvas_height, unsigned canvas_width)
{
    FILE *f_ptr = fopen(f_path, FILE_WRITE);
    if (f_ptr == NULL)
        return -1;

    fprintf(f_ptr, "P3\n%u %u\n255\n", CANVAS_WIDTH, CANVAS_HEIGHT);
    for (unsigned y = 0; y < canvas_height; y++)
        for (unsigned x = 0; x < canvas_width; x++)
        {
            unsigned color = pixels[y * canvas_width + x];
            COMPONENTS components = extract_components(color);
            fprintf(f_ptr, "%d %d %d\n", components.red, components.green, components.blue);
        }

    fclose(f_ptr);
    return 0;
}

void fill_rect(unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH], unsigned canvas_height, unsigned canvas_width, int x1, int y1, int x2, int y2, unsigned color)
{

    for (int y = y1; y <= y2; y++)
        if (y >= 0 && y < canvas_height)
            for (int x = x1; x <= x2; x++)
                if (x >= 0 && x < canvas_width)
                    pixels[y * canvas_width + x] = color;
}

void fill_circle(unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH], unsigned canvas_height, unsigned canvas_width, int cx, int cy, unsigned r, unsigned color)
{
    int x1 = cx - r, y1 = cy - r;
    int x2 = cx + r, y2 = cy + r;

    for (int y = y1; y < y2; y++)
        if (y >= 0 && y < canvas_height)
            for (int x = x1; x < x2; x++)
                if (x >= 0 && x < canvas_width)
                {
                    int dx = cx - x;
                    int dy = cy - y;

                    if (dx * dx + dy * dy < r * r)
                        pixels[y * canvas_width + x] = color;
                }
}

void fill_ring(unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH], unsigned canvas_height, unsigned canvas_width, int cx, int cy, unsigned r1, unsigned r2, unsigned color)
{
    int x1 = cx - r2, y1 = cy - r2;
    int x2 = cx + r2, y2 = cy + r2;

    for (int y = y1; y < y2; y++)
        if (y >= 0 && y < canvas_height)
            for (int x = x1; x < x2; x++)
                if (x >= 0 && x < canvas_width)
                {
                    int dx = cx - x;
                    int dy = cy - y;

                    int d_sq = dx * dx + dy * dy;

                    if (d_sq > r1 * r1 && d_sq < r2 * r2)
                        pixels[y * canvas_width + x] = color;
                }
}

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void fill_line(unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH], unsigned canvas_height, unsigned canvas_width, int x1, int y1, int x2, int y2, unsigned color)
{
    // y = kx + n
    // k = (y2 - y1) / (x2 - x1)

    int dy = y2 - y1;
    int dx = x2 - x1;
    if (x1 > x2)
    {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    if (dx != 0)
    {
        for (int x = x1; x <= x2; x++)
            if (x >= 0 && x < canvas_width)
            {
                int current_y = ((double)dy / dx) * (x - x1) + y1;
                int next_y = ((double)dy / dx) * ((x + 1) - x1) + y1;

                if (current_y > next_y)
                    swap(&current_y, &next_y);

                for (int y = current_y; y <= next_y; y++)
                    if (y >= 0 && y <= canvas_height)
                        pixels[y * canvas_width + x] = color;
            }
        return;
    }

    for (int y = y1; y <= y2; y++)
        pixels[y * canvas_width + x1] = color;
}

int main()
{
    unsigned pixels[CANVAS_HEIGHT * CANVAS_WIDTH] = {COLOR_BLACK};
    char *f_path = "example.ppm";

    fill_rect(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0, 0, CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1, COLOR_BLACK);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0, 0, CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1, COLOR_RED);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0, CANVAS_HEIGHT - 1, CANVAS_WIDTH - 1, 0, COLOR_BLUE);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0, CANVAS_HEIGHT / 2, CANVAS_WIDTH - 1, CANVAS_HEIGHT / 2, COLOR_GREEN);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0, 0, CANVAS_WIDTH / 4, CANVAS_HEIGHT - 1, COLOR_GREEN);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, CANVAS_WIDTH - 1, 0, CANVAS_WIDTH * 3 / 4, CANVAS_HEIGHT - 1, COLOR_GREEN);
    fill_line(pixels, CANVAS_HEIGHT, CANVAS_WIDTH, CANVAS_WIDTH / 2, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT - 1, COLOR_GREEN);
    write_ppm(f_path, pixels, CANVAS_HEIGHT, CANVAS_WIDTH);

    return 0;
}