#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "datastruct.hpp"
#include "image.hpp"
#include "geo.hpp"
#include "grid.hpp"

using namespace std;

bool find_path(GridSquare gs[GRID_HEIGHT*GRID_WIDTH], int start, int finish, List *path, int max_width, int max_height)
{
	List		open;
	List		closed;
	int		finish_x, finish_y;
	
	bool		path_found;

	finish_x = finish % max_width;
	finish_y = finish / max_width;
	
	
	open.insert(start);
	gs[start].set_parent(-1);	
	gs[start].set_f(0);
	gs[start].set_g(0);

	for(; ;)
	{
		if(open.empty())
		{
			path_found = false;
			break;
		}
		
		// A. Select square with lowest F-score from open list
		int current = open.top();
		
		// B. Move current square-id from open to closed list 
		closed.insert(current);
		open.remove(current);
		if(current == finish)
		{
			path_found = true;
			break;
		}
			
		// C. Add valid neighbors of current square to open list + calculate their F-score
		for(int x = gs[current].get_x() - 1; x < gs[current].get_x() + 2; x++)
		{
			for(int y = gs[current].get_y() - 1; y < gs[current].get_y() + 2; y++)
			{
				int selected = x + (y * max_width);

				if(	selected != current &&
					x > -1 &&
				       	x < max_width &&
					y > -1 &&
				       	y < max_height &&
					gs[selected].is_traversable() &&
					!closed.contains(selected))
				{
					if(open.contains(selected))					
					{
						if(	(x == gs[current].get_x() - 1 && y == gs[current].get_y() - 1) ||
							(x == gs[current].get_x() - 1 && y == gs[current].get_y() + 1) ||
							(x == gs[current].get_x() + 1 && y == gs[current].get_y() - 1) ||
							(x == gs[current].get_x() + 1 && y == gs[current].get_y() + 1))
						{
							if(gs[selected].get_g() > gs[current].get_g() + DIA_WEIGHT + gs[selected].get_slope())
							{
								gs[selected].set_parent(current);
								gs[selected].set_g(gs[current].get_g() + DIA_WEIGHT + gs[selected].get_slope());
							}
								
						} else
						{
							if(gs[selected].get_g() > gs[current].get_g() + WEIGHT + gs[selected].get_slope())
							{
								gs[selected].set_parent(current);
								gs[selected].set_g(gs[current].get_g() + WEIGHT + gs[selected].get_slope());
							}
						}
						gs[selected].set_f(gs[selected].get_h() + gs[selected].get_g());
					} else
					{
						gs[selected].set_parent(current);

						if(	(x == gs[current].get_x() - 1 && y == gs[current].get_y() - 1) ||
							(x == gs[current].get_x() - 1 && y == gs[current].get_y() + 1) ||
							(x == gs[current].get_x() + 1 && y == gs[current].get_y() - 1) ||
							(x == gs[current].get_x() + 1 && y == gs[current].get_y() + 1))
							gs[selected].set_g(gs[current].get_g() + DIA_WEIGHT + gs[selected].get_slope());
						else
							gs[selected].set_g(gs[current].get_g() + WEIGHT + gs[selected].get_slope());

						int sel_y = selected / max_width;
						int sel_x = selected % max_width;
						gs[selected].set_h(distance_heuristic(sel_x, sel_y, finish_x, finish_y));
						gs[selected].set_f(gs[selected].get_g() + gs[selected].get_h());
						insert_sorted(&open, selected, gs[selected].get_f(), gs);

					}
				}
			}
		}
	}
	
	int current;
	if(path_found)
	{
		current = finish;
		while(gs[current].get_parent() != -1)
		{
			path->insert(current);
			current = gs[current].get_parent();
		}
	} else
	{
		int smallest = 65536, i;
		current = -1;
		while((i = closed.walkthrough()) != -1)
		{
			if(gs[i].get_h() < smallest)
			{
				current = i;
				smallest = gs[i].get_h();
			}
		}
		
		while(gs[current].get_parent() != -1)
		{
			path->insert(current);
			current = gs[current].get_parent();
		}
	}	
	return path_found;
}

int difference(int a, int b)
{
	return a > b? a - b: b - a;
}

int distance_heuristic(int x, int y, int destx, int desty)
{
	int distx, disty;
	
	distx = difference(destx, x);
	disty = difference(desty, y);
	
	if(distx > disty)
		return ((distx-disty) * WEIGHT) + (disty * DIA_WEIGHT);
	else
		return ((disty-distx) * WEIGHT) + (distx * DIA_WEIGHT);
}

void insert_sorted(List *l, int id, int score, GridSquare gs[])
{
	if(l->empty())
	{
		l->insert(id);
	} else
	{
		
		l->set_active_to_first();
		bool 	inserted = false;
		int 	i;
		int 	j = 0;
		while( (i = l->walkthrough()) != -1)
		{
			if(score < gs[i].get_f())
			{
				l->insert(id, j);
				inserted = true;
				break;
			}
			j++;
		}
		if(!inserted)
			l->insert(id, i);
	}
}

Grid::Grid()
{
	has_heightmap = false;
}

Grid::~Grid()
{
}

void Grid::load(int grid[GRID_HEIGHT][GRID_WIDTH])
{
	int i = 0;
	for(int y = 0; y < GRID_HEIGHT; y++)
		for(int x = 0; x < GRID_WIDTH; x++)
			small_squares[i++] = GridSquare(x, y, !grid[y][x]);
}

void Grid::load_map(char *filename)
{
	ImageTGA	img;
	img.load(filename);

	int i = 0;
	for(int y = 0; y < GRID_HEIGHT; y++)
	{
		for(int x = 0; x < GRID_WIDTH; x++)
		{	
			if(img.pixel(x, y).b > 0)
				small_squares[i++] = GridSquare(x, y, 0);
			else
				small_squares[i++] = GridSquare(x, y, 1);
		}
	}
}

void Grid::load_heightmap(char *filename)
{
	ImageTGA	img;
	img.load(filename);

	for(int y = 0; y < GRID_HEIGHT + 1; y++)
	{
		for(int x = 0; x < GRID_WIDTH + 1; x++)
		{
			vertices[x][y].origin.x = (GLfloat) x;
			vertices[x][y].origin.y = (GLfloat) y;
			vertices[x][y].origin.z = (GLfloat) (img.pixel(x, y).r / 10.0);
		}
	}

	for(int y = 0; y < GRID_HEIGHT; y++)
	{
		for(int x = 0; x < GRID_WIDTH; x++)
		{
			landscape[x][y].add_vertex(&vertices[x][y].origin);
			landscape[x][y].add_vertex(&vertices[x+1][y].origin);
			landscape[x][y].add_vertex(&vertices[x+1][y+1].origin);
			landscape[x][y].add_vertex(&vertices[x][y+1].origin);
		}
	}
	
	for(int y = 1; y < GRID_HEIGHT; y++)
	{
		for(int x = 1; x < GRID_WIDTH; x++)
		{
			vertices[x][y].direction = 	(
								landscape[x-1][y-1].get_normal() +
								landscape[x-1][y-1].get_normal() +
								landscape[x-1][y-1].get_normal() +
								landscape[x-1][y-1].get_normal()
							) / 4.0;
		}
	}
	
	for(int y = 0; y < GRID_HEIGHT; y++)
	{
		for(int x = 0; x < GRID_WIDTH; x++)
		{
			landscape[x][y].add_normal(&vertices[x][y].direction);
			landscape[x][y].add_normal(&vertices[x+1][y].direction);
			landscape[x][y].add_normal(&vertices[x+1][y+1].direction);
			landscape[x][y].add_normal(&vertices[x][y+1].direction);
		}
	}

	Vec3 up;
	up.x = 0.0;
	up.y = 0.0;
	up.z = 1.0;
	for(int i = 0, x = 0; x < GRID_WIDTH; x++)
	{
		for(int y = 0; y < GRID_HEIGHT; y++)
		{
			
			small_squares[i++].set_slope( (int) (acos(dot_product(up, landscape[x][y].get_normal())) / DEG2RAD));
/*			if(small_squares[i-1].get_slope() > 40)
				small_squares[i-1].set_traversability(0);*/
		}
	}

	for(int x = 0; x < BIG_GRID_DIVISOR; x++)
	{
		for(int y = 0; y < BIG_GRID_DIVISOR; y++)
		{	
			long int average_slope = 0;
			for(int x2 = x * (GRID_WIDTH / BIG_GRID_DIVISOR); x2 < (x * (GRID_WIDTH / BIG_GRID_DIVISOR)) + (GRID_WIDTH / BIG_GRID_DIVISOR); x2++)
			{
				for(int y2 = y * (GRID_HEIGHT / BIG_GRID_DIVISOR); y2 < (y * (GRID_HEIGHT / BIG_GRID_DIVISOR)) + (GRID_HEIGHT / BIG_GRID_DIVISOR); y2++)
				{
					int i = x2 + (y2 * GRID_WIDTH);
					if(small_squares[i].is_traversable())
					{
						average_slope += small_squares[i].get_slope();
					}
					else
					{
						average_slope += 100;
					}
				}
			}
			big_squares[x + (y * BIG_GRID_DIVISOR)] = GridSquare(x, y, 1);
			big_squares[x + (y * BIG_GRID_DIVISOR)].set_slope(average_slope / ((GRID_WIDTH / BIG_GRID_DIVISOR) * (GRID_HEIGHT / BIG_GRID_DIVISOR)));
		}
	}

	for(int x = 0; x < HUGE_GRID_DIVISOR; x++)
	{
		for(int y = 0; y < HUGE_GRID_DIVISOR; y++)
		{	
			long int average_slope = 0;
			for(int x2 = x * (GRID_WIDTH / HUGE_GRID_DIVISOR); x2 < (x * (GRID_WIDTH / HUGE_GRID_DIVISOR)) + (GRID_WIDTH / HUGE_GRID_DIVISOR); x2++)
			{
				for(int y2 = y * (GRID_HEIGHT / HUGE_GRID_DIVISOR); y2 < (y * (GRID_HEIGHT / HUGE_GRID_DIVISOR)) + (GRID_HEIGHT / HUGE_GRID_DIVISOR); y2++)
				{
					int i = x2 + (y2 * GRID_WIDTH);
					if(small_squares[i].is_traversable())
					{
						average_slope += small_squares[i].get_slope();
					}
					else
					{
						average_slope += 100;
					}
				}
			}
			huge_squares[x + (y * HUGE_GRID_DIVISOR)] = GridSquare(x, y, 1);
			huge_squares[x + (y * HUGE_GRID_DIVISOR)].set_slope(average_slope / ((GRID_WIDTH / HUGE_GRID_DIVISOR) * (GRID_HEIGHT / HUGE_GRID_DIVISOR)));
		}
	}
	
	has_heightmap = true;
}

void Grid::to_screen()
{
	for(int i = 0, x = 0; x < GRID_WIDTH; x++)
	{
		for(int y = 0; y < GRID_HEIGHT; y++, i++)
		{
			
			if(has_heightmap)
			{
				if(small_squares[i].is_traversable())
				{
					landscape[x][y].to_screen_height();
				}
				else
				{
					glColor3f(0.0, 0.0, 0.8);
					landscape[x][y].to_screen();
				}
			} else
			{
				glBegin(GL_QUADS);
				glVertex3i(x, y, 0);
				glVertex3i(x+1, y, 0);
				glVertex3i(x+1, y+1, 0);
				glVertex3i(x, y+1, 0);
				glEnd();
			}
		}
	}
}

int Grid::nearest_traversable(int target, int range)
{
	for(int x = (target % GRID_WIDTH) - range; x < (target % GRID_WIDTH) + range; x++)
	{
		for(int y = (target / GRID_WIDTH) - range; y < (target / GRID_WIDTH) + range; y++)
		{
			if(x > - 1 && x < GRID_WIDTH && y > - 1 && y < GRID_HEIGHT && small_squares[x + (y*GRID_WIDTH)].is_traversable())
				return x + (y * GRID_WIDTH);
		}
	}
	return nearest_traversable(target, range + 1);
}

void Grid::get_path(int x0, int y0, int x1, int y1, List *path)
{
	int target = x1 + (y1*GRID_WIDTH);
	path->destroy();
	
	if(small_squares[target].is_traversable())
		find_path(small_squares, x0 + (y0*GRID_WIDTH), target, path, GRID_WIDTH, GRID_HEIGHT);
	else
		find_path(small_squares, x0 + (y0*GRID_WIDTH), nearest_traversable(target, 1), path, GRID_WIDTH, GRID_HEIGHT);
}

void Grid::get_big_path(int x0, int y0, int x1, int y1, List *path)
{
	x0 /= (GRID_WIDTH / BIG_GRID_DIVISOR); 
	x1 /= (GRID_WIDTH / BIG_GRID_DIVISOR); 
	y0 /= (GRID_HEIGHT / BIG_GRID_DIVISOR); 
	y1 /= (GRID_HEIGHT / BIG_GRID_DIVISOR); 
	
	path->destroy();
	find_path(big_squares, x0 + (y0 * BIG_GRID_DIVISOR), x1 + (y1 * BIG_GRID_DIVISOR), path, BIG_GRID_DIVISOR, BIG_GRID_DIVISOR);
}

void Grid::get_huge_path(int x0, int y0, int x1, int y1, List *path)
{
	x0 /= (GRID_WIDTH / HUGE_GRID_DIVISOR); 
	x1 /= (GRID_WIDTH / HUGE_GRID_DIVISOR); 
	y0 /= (GRID_HEIGHT / HUGE_GRID_DIVISOR); 
	y1 /= (GRID_HEIGHT / HUGE_GRID_DIVISOR); 
	
	path->destroy();
	find_path(huge_squares, x0 + (y0 * HUGE_GRID_DIVISOR), x1 + (y1 * HUGE_GRID_DIVISOR), path, HUGE_GRID_DIVISOR, HUGE_GRID_DIVISOR);
}

void Grid::show_path(List *small_path, List *big_path, List *huge_path)
{
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(3.0);	
	
	glBegin(GL_POINTS);
	for(int i = 0; i < small_path->size(); i++)
	{
		int x = small_path->index(i) / GRID_WIDTH;
		int y = small_path->index(i) % GRID_WIDTH;
		Ray r;
		r.origin.x = (GLfloat) x + 0.5;
		r.origin.y = (GLfloat) y + 0.5;
		r.origin.z = 100.0;
		r.direction = 0.0;
		r.direction.z = -1.0;
		
		glVertex3f((GLfloat) x + 0.5,(GLfloat) y + 0.5, 100.0 - landscape[x][y].distance_to_ray(r));
	}
	glEnd();
	
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for(int i = 0; i < big_path->size(); i++)
	{
		int x = (big_path->index(i) / BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
		int y = (big_path->index(i) % BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
		Ray r;
		r.origin.x = (GLfloat) x + 0.5;
		r.origin.y = (GLfloat) y + 0.5;
		r.origin.z = 100.0;
		r.direction = 0.0;
		r.direction.z = -1.0;
		
		glVertex3f((GLfloat) x + 0.5,(GLfloat) y + 0.5, 100.0 - landscape[x][y].distance_to_ray(r));
	}
	glEnd();
	
	glColor3f(1.0, 0.0, 1.0);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for(int i = 0; i < huge_path->size(); i++)
	{
		int x = (huge_path->index(i) / HUGE_GRID_DIVISOR) * (GRID_WIDTH / HUGE_GRID_DIVISOR) + (GRID_WIDTH / (HUGE_GRID_DIVISOR*2));
		int y = (huge_path->index(i) % HUGE_GRID_DIVISOR) * (GRID_WIDTH / HUGE_GRID_DIVISOR) + (GRID_WIDTH / (HUGE_GRID_DIVISOR*2));
		Ray r;
		r.origin.x = (GLfloat) x + 0.5;
		r.origin.y = (GLfloat) y + 0.5;
		r.origin.z = 100.0;
		r.direction = 0.0;
		r.direction.z = -1.0;
		
		glVertex3f((GLfloat) x + 0.5,(GLfloat) y + 0.5, 100.0 - landscape[x][y].distance_to_ray(r));
	}
	glEnd();

}

bool Grid::is_traversable(int x, int y)
{
	return small_squares[(y * GRID_WIDTH) + x].is_traversable();
}

Plane Grid::get_plane(GLfloat x, GLfloat y)
{
	int x2 = (int) x;
	int y2 = (int) y;
	if(x2 > -1 && x2 < GRID_WIDTH && y2 > -1 && y2 < GRID_HEIGHT)
		return landscape[x2][y2];
}

void Grid::highlight(int x, int y, GLfloat t)
{
	if(is_traversable(y, x))
		glColor3f(0.0, 1.0, 0.0);
	else
		glColor3f(1.0, 0.0, 0.0);
		
	Ray 	r;
	Vec3 	n;
	r.origin.x =  (GLfloat) x + 0.5;
	r.origin.y =  (GLfloat) y + 0.5;
	r.origin.z = 100.0;
	r.direction = 0.0;
	r.direction.z = -1.0;
		
	GLfloat z = 100.0 - landscape[x][y].distance_to_ray(r);

	glTranslatef(x+0.5, y+0.5, z);
	glRotatef(t, 0.0, 0.0, 1.0);
	glPushMatrix();
	glBegin(GL_TRIANGLES);

		n.x = 0.5; n.y = 0.5; n.z = 0.0;
		n.set_screen_normal();
		glVertex3f( -0.5, -0.5, 0.0); 
		glVertex3f( 0.0, 0.0, 3.0); 
		glVertex3f( 0.5, 0.5, 0.0); 
		
		n.x = -0.5; n.y = 0.5; n.z = 0.0;
		n.set_screen_normal();
		glVertex3f( 0.5,  -0.5, 0.0); 
		glVertex3f( 0.0, 0.0, 3.0); 
		glVertex3f( -0.5, 0.5, 0.0); 
	glEnd();
	glPopMatrix();
}
		
