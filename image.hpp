#ifndef __TGA_HPP
#define __TGA_HPP

#ifndef IN
#define IN
#endif

#define W 0
#define H 1

#define R 0
#define G 1
#define B 2
#define A 3

#define MAX_ROWS 768
#define MAX_COLS 1024

union charToShort
{
	/* 1 x 2 byte variable */
	unsigned short s;
	/* 2 x 1 byte variables */
	unsigned char  b[2];
};

struct Pixel
{
	GLubyte	r;
	GLubyte	g;
	GLubyte	b;
	GLubyte	a;
};

class Prow
{
	public:
		Prow();
		~Prow();
		
		void 	add_pixel(int r, int g, int b, int a);
		Pixel	get_pixel(int x);

	private:
		Pixel	pixel[MAX_COLS];
		int	n_pixels;	
};

class ImageTGA
{
	public:
		ImageTGA();
		~ImageTGA();
		void 	load(char *filename);
		int  	dimensions(int which); 
		Pixel	pixel(int x, int y);	
		bool	is_loaded()		{ return !load_failure; }
		
	private:
		bool 	load_failure;
		int	w;
		int	h;
		bool	r2l;
		bool	t2b;
		Prow	pixel_row[MAX_ROWS];		
};

bool 	tga_check_signature(char *sig);
int 	CheckByteOrder();
void 	Swap2Bytes(IN void *ptr);

#endif /* __TGA_HPP */
