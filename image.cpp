#include <iostream>
#include <fstream>
#include <string>
#include <GL/glut.h>
#include "image.hpp"

using namespace std;

bool tga_check_signature(char *sig)
{
	string check;
	check = sig;
	if(check.substr(0,16) == "TRUEVISION-XFILE")
		return true;
	return false;
}

int CheckByteOrder()
{
	/* Create a 2byte check variable */
	union charToShort cs;

	/* assign 1 to the 2byte variable */
	cs.s = 1;

	/* check the first byte */
	if(cs.b[0] == 0)
		return 1;
	
	return 0;
}


void Swap2Bytes(IN void *ptr)
{
	/* assign a pointer to the 2byte variable */
	unsigned char *pcs = (unsigned char*)ptr;

	/* swap the bytes around */
	unsigned char temp = pcs[0];
	pcs[0] = pcs[1];
	pcs[1] = temp;
}

Prow::Prow()
{
	n_pixels = 0;
}

Prow::~Prow()
{
}

void Prow::add_pixel(int r, int g, int b, int a)
{
	if(n_pixels < MAX_COLS)
	{
		pixel[n_pixels].r = (GLubyte) r;
		pixel[n_pixels].g = (GLubyte) g;
		pixel[n_pixels].b = (GLubyte) b;
		pixel[n_pixels].a = (GLubyte) a;
		n_pixels++;
	}
}

Pixel	Prow::get_pixel(int x)
{
	if(x < n_pixels)
		return pixel[x];
}

ImageTGA::ImageTGA()
{
	load_failure = false;
	w = 0;
	h = 0;
}

ImageTGA::~ImageTGA()
{
}

void ImageTGA::load(char *filename)
{	
	ifstream 	infile;
	int		length;
	char		*sig = new char[16];
	
	infile.open(filename, ios::binary);	
	if(!infile.is_open())
	{
		cout << "Error loading file: " << filename << endl;
		delete sig;
		load_failure = true;
	}
	
	infile.seekg(0, ios::end);
	length = infile.tellg();
	infile.seekg(length - 18);
	infile.read(sig, 16);

	if(tga_check_signature(sig) && !load_failure)
	{
		char 		IDlen, cMapType, imgType, cMapEntrySize, pDepth, imgDescriptor;
		unsigned short	cMapOffset, cMapLen, xOff, yOff, width, height;
		int		dump, bpp;
		
		cout << "Loading TGA-file:\t\t" << filename << "..." << endl;
		infile.seekg(0, ios::beg); 
		infile.read(&IDlen, 1);
		infile.read(&cMapType, 1);
		infile.read(&imgType, 1);
		infile.read((char*)&cMapOffset, 2);	
		infile.read((char*)&cMapLen, 2);	
		infile.read(&cMapEntrySize, 1);
		infile.read((char*)&xOff, 2);
		infile.read((char*)&yOff, 2);
		infile.read((char*)&width, 2);
		infile.read((char*)&height, 2);
		infile.read(&pDepth, 1);
		infile.read(&imgDescriptor, 1);

		if(CheckByteOrder())
		{
			Swap2Bytes((void*)&cMapOffset);
			Swap2Bytes((void*)&cMapLen);
			Swap2Bytes((void*)&xOff);
			Swap2Bytes((void*)&yOff);
			Swap2Bytes((void*)&width);
			Swap2Bytes((void*)&height);
		}
		infile.read((char*)&dump,(int) IDlen);

		bpp = (int) pDepth / 8;
		r2l = (bool) ((imgDescriptor >> 3) & 1);
		t2b = (bool) ((imgDescriptor >> 2) & 1);
			
		if(imgType == 2 || imgType == 10)
		{
			w = (int) width;
			h = (int) height;
			if(imgType == 2)
			{
				for(int y = 0; y < height; y++)
				{
					for(int x = 0; x < width; x++)
					{
						int tp[4];
						for(int i = 0; i < 4; i++)
						{
							unsigned char c;
							infile.read((char*)&c, bpp/4);
							tp[i] = (int) c;
						}
						if(y < MAX_ROWS)
							pixel_row[y].add_pixel(tp[2], tp[1], tp[0], tp[3]);
					}
				}
			} else
			{
				unsigned char byte;
				int x = 0;
				int y = 0;
				while(y < height)
				{
					infile.read((char*)&byte, 1);
					if( (byte & 0x80) == 0)
					{
						byte++;
						for(int j = 0; j < byte; ++j)
						{
							int tp[4];
							for(int i = 0; i < 4; i++)
							{
								unsigned char c;
								infile.read((char*)&c, bpp/4);
								tp[i] = (int) c;
							}
							if(y < MAX_ROWS)
								pixel_row[y].add_pixel(tp[2], tp[1], tp[0], tp[3]);
							if(x < width)
							{
								x++;
							}
						}
					} else
					{
						int tp[4];
						byte -= 0x7F;
						for(int i = 0; i < 4; i++)
						{
							unsigned char c;
							infile.read((char*)&c,bpp/4);
							tp[i] = (int) c;
						}
						for(int j = 0; j < byte; ++j)
						{
							if(y < MAX_ROWS)
								pixel_row[y].add_pixel(tp[2], tp[1], tp[0], tp[3]);
							if(x < width)
							{
								x++;
							}
						}			
					}
					if(x == width)
					{
						x= 0;
						y++;
					}
					if(y == height)
						break;
				}
			}
		} else
		{
			cout << "Error loading file: " << filename << " -- only true color images supported!" << endl;
			load_failure = true;
		}
	} else
	{
		cout << "Error loading file: " << filename << " -- incorrect signature" << endl;
		load_failure = true;
	}

	if(infile.is_open())	
		infile.close();	

	delete sig;
}

int ImageTGA::dimensions(int which)
{
	switch(which)
	{
		case W: return w;
		case H: return h;
		default: return -1;
	}
}

Pixel ImageTGA::pixel(int x, int y)
{
	if(y < sizeof(pixel_row))
		return pixel_row[y].get_pixel(x);
}	
