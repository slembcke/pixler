#include <stdbool.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "shared.h"

#define MAX 128

typedef struct {uint x, y;} uvec;

static float intersect_parabolas(uvec a, uvec b){
	int num = (a.x*a.x + a.y) - (b.x*b.x + b.y);
	int denom = 2*(a.x - b.x);
	return (float)num/(float)denom;
}

// TODO hardcoded sizes.
uvec v[1024];
float z[1024];

static void build_hull(uint edt[], uint count){
	v[0] = (uvec){0, edt[0]};
	z[0] = -MAX, z[1] = MAX;
	
	uint k = 0;
	for(uint i = 1; i < count; i++){
		uvec p = v[k], q = {i, edt[i]};
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

static void march_parabolas(uint count, uint output[], uint stride){
	uint j = 0;
	for(uint i = 0; i < count; i++){
		while(z[j + 1] < i) j += 1;
		int dx = i - v[j].x;
		output[i*stride] = dx*dx + v[j].y;
	}
}

static void scan_pass(uint output[], uint input[], uint w, uint h){
	for(uint row = 0; row < h; row++){
		build_hull(input + row*w, w);
		march_parabolas(w, output + row, h);
	}
}

static uint *generate_edt(Image image){
	uint edt_len = image.w*image.h;
	uint *buff0 = malloc(2*sizeof(*buff0)*edt_len);
	uint *buff1 = buff0 + edt_len;
	buff1[0] = 0;
	
	// Convert to initial EDT values.
	for(uint i = 0; i < edt_len; i++) buff0[i] = (image.pixels[i] ? MAX : 0);
	
	scan_pass(buff1, buff0, image.w, image.h);
	scan_pass(buff0, buff1, image.h, image.w);
	return realloc(buff0, sizeof(*buff0)*edt_len);
}

int main(int argc, char *argv[]){
	SLIB_ASSERT_HARD(argc == 3, "Usage: %s infile outfile", argv[0]);
	
	FILE *infile = fopen(argv[1], "r");
	SLIB_ASSERT_HARD(infile, "Can't open input file '%s'", argv[1]);
	
	// FILE *outfile = fopen(argv[2], "w");
	// SLIB_ASSERT_HARD(outfile, "Can't open output file '%s'", argv[2]);
	
	Image image = read_png(infile);
	uint *edt = generate_edt(image);
	
	for(uint y = 0; y < image.h; y++){
		for(uint x = 0; x < image.w; x++) printf("%02X ", edt[x + y*image.w]);
		printf("\n");
	}
	
	return EXIT_SUCCESS;
}
