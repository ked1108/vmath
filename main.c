#define _XOPEN_SOURCE 500

#include "vmath.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FPS 15.0

typedef enum {
	OFF = 0,
	ON = 1
} Pixel;

typedef struct {
	int width;
	int height;
	Pixel* buffer;
} Display;

Display d;

void init_display(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	// NOTE: 	To ensure that the number of rows stays even we left and right shift,
	d.height = (w.ws_row >> 1) << 2;
	d.width = w.ws_col;

	d.buffer = malloc(d.width*d.height*sizeof(Pixel));
}

// Character Table
// ------------------------------
// top bot | op
// -------------
//  0   0  |     	Empty
//  0   1  | _   	Bottom Heavy
//  1   0  | ^   	Top Heavy
//  1   1  | @   	Full
// ------------------------------

void render () {
	char buffer_row[d.width];
	char* table = " _^@";
	for(int row = 0; row < (d.height>>1); row++) {
		for(int col = 0; col < d.width; col++) {
			Pixel top = d.buffer[d.width * (2*row) + col];
			Pixel bot = d.buffer[d.width * (2*row + 1) + col];
			buffer_row[col] = table[(top << 1) | bot]; 
		}
		fwrite(buffer_row, sizeof(char), d.width, stdout);
		printf("\n");
	}
}

bool inbound(Vector2 vec) {
	return (bool)(0 <= vec.x && vec.x < d.width && 0 <= vec.y && vec.y < d.height);
}

void draw_circle(Vector2 pos, int radius) {
	Vector2 v;
	Vector2 e = Vector2Add(pos, (Vector2){radius, radius});
	for(v.y = pos.y-radius; v.y < e.y ; v.y++) {
		for(v.x = pos.x-radius; v.x < e.x ; v.x++) {
			if((int)Vector2DistanceSqr(v, pos) <= (radius*radius) && inbound(v)) {
				d.buffer[(int)floor(v.y)*d.width + (int)floor(v.x)] = ON; 
			}
		}
	}
}

void clear(void) {
	memset(d.buffer, OFF, d.width*d.height*4);
}

void reset(void) {
	const char* esc = "\033[";
	const char* clrs = "2J";
	const char* home = "H";
	printf("%s%s", esc, clrs);
	printf("%s%s", esc, home);

}


int main() {
	init_display();
	clear();

	Vector2 gravity = {0, 12};
	Vector2 velocity = {8, 8};

	Vector2 pos = {0, 0};
	const int radius = d.height/4;

	const double dt = 1.0 / FPS;
	while(1) {
		reset();
		clear();

		velocity = Vector2Add(velocity, Vector2Scale(gravity, dt));
		pos = Vector2Add(pos, Vector2Scale(velocity, dt));
		if(pos.y+radius >= d.height) {
			pos.y = d.height - radius;
			velocity.y = -velocity.y * 0.90;
		}
		draw_circle(pos, radius);

		render();
		usleep(1000 * 1000 / FPS);
	}
}
