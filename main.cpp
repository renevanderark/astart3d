#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "datastruct.hpp"
#include "geo.hpp"
#include "grid.hpp"
#include "person.hpp"

using namespace std;

GLfloat	lpos[4] = { 1.0, 1.0, 0.0, 1.0 };
Grid		grid;
GLfloat		t = 0.0;
GLfloat		t2 = 0.0;
GLfloat		rot_acc = 0.0;
GLfloat		zoom = 0.0;
PersonBase		person[MAX_PERSONS];
RequestQueue	small_path_requests;

bool		glsl_support;
bool		showing_paths = true;
int		px = 0, py = 0;
int		last_x, last_y;
int		n_persons = MAX_PERSONS;

void resize(int w, int h)
{
	GLfloat ratio = 1.0 * w / (h > 0 ? h : 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	gluPerspective(35.0, ratio, 1.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0, 0.0, zoom);
	glTranslatef(0.0, -30.0, -435.0);
	glRotatef(-(60.0 + (zoom/20.0)), 1.0, 0.0, 0.0);
	glRotatef(t, 0.0, 0.0, 1.0); 
	glPushMatrix();
		grid.to_screen();
		glDisable(GL_LIGHTING);

		for(int i = 0; i < n_persons; i++)
		{
			if(showing_paths)
				grid.show_path(person[i].get_small_path(), person[i].get_big_path(), person[i].get_huge_path());
			person[i].to_screen(grid.get_plane(person[i].x(), person[i].y()));
		}
		
		glEnable(GL_LIGHTING);
		grid.highlight(py, px, t2);
	glPopMatrix();
	
	glutSwapBuffers();
}

void timer(int var)
{
	glutPostRedisplay();
	t += rot_acc;
	t2 += 1.0;

	for(int i = 0; i < n_persons; i++)
	{
		if(person[i].is_walking())
		{
			int path_request = person[i].walk(&grid);
			
			if(path_request == REQUEST_PATH)
				small_path_requests.push(i);
		}
	}


	for(int i = 0; i < 2; i++)
	{
		if(small_path_requests.top() != -1)
			person[small_path_requests.pop()].find_small_path(&grid);
	}
	 
	glutTimerFunc(10, timer, 1);
}

void mouse_button(int button, int state, int x, int y)
{
	if(state == GLUT_UP && grid.is_traversable(px, py))
	{
		for(int i = 0; i < n_persons; i++)
		{
			person[i].find_path_to(px, py, &grid);
			small_path_requests.push(i);
		}
	}
}

void mouse_motion(int x, int y)
{
	if(y < last_y && px < GRID_WIDTH - 1)
		px++;
	
	if(y > last_y && px > 0)
		px--;
	
	if(x < last_x && py > 0)
		py--;
	
	if(x > last_x && py < GRID_HEIGHT - 1)
		py++;

	last_x = x; last_y = y;
}

void keyboard(unsigned char k, int x, int y)
{
	if(k == 'q' || k == 'Q' || k == 27)
		exit(0);

	if(k == ' ')
	{
		for(int i = 0; i < n_persons; i++)
		{		
			if(person[i].is_walking())
				person[i].stop_walking();
			else
				person[i].start_walking();
		}
	}
	
	if(k == 9)
		showing_paths = !showing_paths;

	if(k == 'Z')
		zoom -= 10.0;
	
	if(k == 'z')
		zoom += 10.0;

	if(k == 'R')
		rot_acc -= 0.05;
	
	if(k == 'r')
		rot_acc += 0.05;
}


void initialize(int argc, char *argv[])
{
	srand(rand());

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(10,10);
	glutInitWindowSize(800,600);
	glutCreateWindow("New Project");
					
/*	glutGameModeString("1024x768:32");
	glutEnterGameMode();*/
	glewInit();

	grid.load_map("./img/map.tga");
	grid.load_heightmap("./img/heightmap2.tga");

	int rt = (int) sqrt(n_persons);
	for(int i = 0; i < n_persons; i++)
	{
		GLfloat x = ((i % rt)*4.0) *((float) rand() / (float) RAND_MAX) * 2.0;
		GLfloat y = ((i / rt)*4.0) *((float) rand() / (float) RAND_MAX) * 2.0;
		if(grid.is_traversable((int) y, (int) x))
		{
			person[i].set_pos(x, y);
		} else
		{
			person[i].set_pos(1.0, 1.0);
		}
				  
	}

	glutMotionFunc(mouse_motion);	
	glutMouseFunc(mouse_button);	
	glutDisplayFunc(display);
	glutTimerFunc(5, timer, 1);

	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);

	glPointSize(2.0);	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glEnable(GL_LIGHT0);
	glClearColor(0.4, 0.4, 1.0, 1.0);
	
	if(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_multitexture)
	{	
		cout << "GLSL supported" << endl;
		glsl_support = true;
	} else
	{
		cout << "No GLSL support" << endl;
		glsl_support = false;
	}
}

void print_errors()
{
	GLenum error;
	const char *error_msg;
	if(error = glGetError() != GL_NO_ERROR)
	{
		error_msg = (const char *) gluErrorString(error);
		cout << error_msg << endl;
	}
}

int main(int argc, char *argv[])
{
	initialize(argc, argv);
	glutMainLoop();
	return 0;
}
