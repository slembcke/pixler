#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

#include "shared.h"

#define MAX 256

typedef struct {uint x, y;} uvec;

static void dbg(){}

static float intersect_parabolas(uvec a, uvec b){
	int num = (a.x*a.x + a.y) - (b.x*b.x + b.y);
	int denom = 2*(a.x - b.x);
	return (float)num/(float)denom;
}

// TODO hardcoded sizes.
uvec v[1024];
float z[1024];

static void build_hull(uint dist[], uint count){
	v[0] = (uvec){0, dist[0]};
	z[0] = -MAX, z[1] = MAX;
	
	uint k = 0;
	for(uint i = 1; i < count; i++){
		uvec p = v[k], q = {i, dist[i]};
		float s = intersect_parabolas(p, q);
		
		while(s <= z[k]){
			k -= 1, p = v[k], s = intersect_parabolas(p, q);
		}
		
		k += 1;
		v[k] = q;
		z[k] = s;
		z[k + 1] = i + MAX;
	}
}

static void march_parabolas(uint dist[], uint count, uint out[], uint stride){
	uint j = 0;
	for(uint i = 0; i < count; i++){
		while(z[j + 1] < i) j += 1;
		int dx = i - v[j].x;
		out[i*stride] = dx*dx + v[j].y;
	}
}

static void scan_pass(uint *output, uint *input, uint w, uint h){
	for(uint row = 0; row < h; row++){
		uint *image_row = input + row*w;
		build_hull(image_row, w);
		march_parabolas(image_row, w, output + row, w);
	}
}

static uint *generate_edt(uint w, uint h){
	uint edt_size = sizeof(uint)*w*h;
	uint *buff0 = malloc(2*edt_size);
	uint *buff1 = buff0 + edt_size;
	
	for(uint y = 0; y < h; y++){
		for(uint x = 0; x < w; x++) buff0[x + y*w] = MAX;
	}
	buff0[0] = 0;
	buff0[255] = 0;
	
	scan_pass(buff1, buff0, w, h);
	scan_pass(buff0, buff1, h, w);
	return realloc(buff0, edt_size);
}

int main(int argc, char *argv[]){
	SLIB_ASSERT_HARD(argc == 3, "Usage: %s infile outfile", argv[0]);
	
	FILE *infile = fopen(argv[1], "r");
	SLIB_ASSERT_HARD(infile, "Can't open input file '%s'", argv[1]);
	
	// FILE *outfile = fopen(argv[2], "w");
	// SLIB_ASSERT_HARD(outfile, "Can't open output file '%s'", argv[2]);
	
	// Image image = read_png(infile);
	// for(uint r = 0; r < image.h/8; r++) {
	// 	for(uint c = 0; c < image.w/8; c++) {
	// 		const u8 *pixels = image.pixels + 8*(c + r*image.w);
	// 		u8 tile[16] = {};
	// 		tile_to_chr(pixels, image.w, tile);
	// 		fwrite(&tile, sizeof(tile), 1, outfile);
	// 	}
	// }
	
	Image image = {.w = 16, .h = 16};
	uint *edt = generate_edt(image.w, image.h);
	
	for(uint y = 0; y < image.h; y++){
		for(uint x = 0; x < image.w; x++) printf("% 4d, ", edt[x + y*image.w]);
		printf("\n");
	}
	
	return EXIT_SUCCESS;
}
