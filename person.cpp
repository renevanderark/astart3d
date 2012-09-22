#include <math.h>
#include <iostream>
#include <GL/glut.h>
#include "datastruct.hpp"
#include "geo.hpp"
#include "grid.hpp"
#include "person.hpp"

#define N_PERSONS	256
#define	NO_REQUEST	0
#define REQUEST_PATH	1

using namespace std;

PersonBase::PersonBase()
{
	big_target_x = small_target_x = xPos = 0.0;
	big_target_y = small_target_y = yPos = 0.0;
	real_target_x = real_target_y = xPath_pending = yPath_pending = 0;
	
	walking	= false;
}

PersonBase::~PersonBase()
{
}

void PersonBase::to_screen(Plane on_plane)
{
	Ray 		r;
	GLfloat		zPos;
	
	r.origin.x =  ((int) xPos + 1.0) - (xPos - (int) xPos);
	r.origin.y =  ((int) yPos + 1.0) - (yPos - (int) yPos);
	
	r.origin.z = 100.0;
	
	r.direction = 0.0;
	r.direction.z = -1.0;

	zPos = 100.0 - on_plane.distance_to_ray(r);
	
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(xPos, yPos, zPos);
	glVertex3f(xPos, yPos, zPos + 0.8);
	glEnd();
}

int PersonBase::walk(Grid *g)
{	

	if(	yPos > big_target_y - BIG_GRID_TBOUND && 
		yPos < big_target_y + BIG_GRID_TBOUND &&
		xPos > big_target_x - BIG_GRID_TBOUND && 
		xPos < big_target_x + BIG_GRID_TBOUND)
	{	
		if(big_path.size() > 1)
		{
			int v = big_path.pop();
			big_target_x = ((v / BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR)) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
			big_target_y = ((v % BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR)) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
			v = big_path.top();
			xPath_pending = (v % BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
			yPath_pending = (v / BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
			return REQUEST_PATH;
		} else /* big_path.size() == 0 || 1 */
		{
			big_path.pop(); /* Now size is definitely 0 */
			if(huge_path.size() > 0)
			{
				int v = huge_path.pop();
				int x = (v % HUGE_GRID_DIVISOR) * (GRID_WIDTH / HUGE_GRID_DIVISOR) + (GRID_WIDTH / (HUGE_GRID_DIVISOR*2));
				int y = (v / HUGE_GRID_DIVISOR) * (GRID_WIDTH / HUGE_GRID_DIVISOR) + (GRID_WIDTH / (HUGE_GRID_DIVISOR*2));
				find_big_path(x, y, g);
				return REQUEST_PATH;
			} else 
			{
				if(xPath_pending != real_target_x && yPath_pending != real_target_y)
				{
					xPath_pending = real_target_x;
					yPath_pending = real_target_y;
					return REQUEST_PATH;
				}
			}
		}
	}
	
	if(	yPos > small_target_y - 0.05 && yPos < small_target_y + 0.05 &&
		xPos > small_target_x - 0.05 && xPos < small_target_x + 0.05)
	{
		if(small_path.size() == 0 && big_path.size() == 0 && huge_path.size() == 0)
			walking = false;
		
		int v = small_path.pop();
		if(v > -1)
		{
			small_target_x = (GLfloat) (v / GRID_WIDTH) + 0.5;
			small_target_y = (GLfloat) (v % GRID_WIDTH) + 0.5;
		}
	}

	GLfloat dx = small_target_x - xPos;
	GLfloat dy = small_target_y - yPos;
	GLfloat dr = dx*dx + dy*dy;

	if(dr > 0.0)
	{
		xPos += (dx / sqrt(dr)) / 10.0;
		yPos += (dy / sqrt(dr)) / 10.0;
	}
	
	return NO_REQUEST;
}

void PersonBase::find_path_to(int x, int y, Grid *g)
{
	real_target_x = x;
	real_target_y = y;
	
	find_huge_path(x, y, g);
}

void PersonBase::find_huge_path(int x, int y, Grid *g)
{	
	g->get_huge_path((int) yPos, (int) xPos, x, y, &huge_path);

	if(huge_path.size() == 0)
	{
		find_big_path(x, y, g);
	} else
	{
		int v = huge_path.pop();
		int x1 = (v % HUGE_GRID_DIVISOR) * (GRID_WIDTH / HUGE_GRID_DIVISOR) + (GRID_WIDTH / (HUGE_GRID_DIVISOR*2)); 
		int y1 = (v / HUGE_GRID_DIVISOR) * (GRID_HEIGHT / HUGE_GRID_DIVISOR) + (GRID_HEIGHT / (HUGE_GRID_DIVISOR*2)); 	

		find_big_path(x1, y1, g);
	}
}	

void PersonBase::find_big_path(int x, int y, Grid *g)
{
	g->get_big_path((int) yPos, (int) xPos, x, y, &big_path);
	
	if(big_path.size() == 0)	
	{
		xPath_pending = x;
		yPath_pending = y;
	} else 
	{
		int v = big_path.pop();
		big_target_y = ((v % BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR)) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2));
		big_target_x = ((v / BIG_GRID_DIVISOR) * (GRID_HEIGHT / BIG_GRID_DIVISOR)) + (GRID_HEIGHT / (BIG_GRID_DIVISOR*2));
	
		v = big_path.pop();	
		if(v > -1)
		{	
			xPath_pending = (v % BIG_GRID_DIVISOR) * (GRID_WIDTH / BIG_GRID_DIVISOR) + (GRID_WIDTH / (BIG_GRID_DIVISOR*2)); 
			yPath_pending = (v / BIG_GRID_DIVISOR) * (GRID_HEIGHT / BIG_GRID_DIVISOR) + (GRID_HEIGHT / (BIG_GRID_DIVISOR*2)); 	
		} else
		{
			xPath_pending = (int) big_target_y;
			yPath_pending = (int) big_target_x;
		}
	}	
}

void PersonBase::find_small_path(Grid *g)
{
	g->get_path((int) yPos, (int) xPos, xPath_pending, yPath_pending, &small_path);
	if(small_path.size() > 0)
	{
		small_target_x = (small_path.top() / GRID_WIDTH) + 0.5;
		small_target_y = (small_path.top() % GRID_WIDTH) + 0.5;
	}
}
