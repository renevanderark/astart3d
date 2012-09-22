#include <math.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "geo.hpp"

using namespace std;


GLdouble dot_product(Vec3 a, Vec3 b)
{
	return 	(a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 cross_product(Vec3 a, Vec3 b)
{
	Vec3 cp;
	
	cp.x = (a.y * b.z) - (a.z * b.y);
	cp.y = (a.z * b.x) - (a.x * b.z);
	cp.z = (a.x * b.y) - (a.y * b.x);
	
	return cp;
}

Vec3 rotate_vec3(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, Vec3 vec)
{
	Matrix3x3 m;
	
	angle *= DEG2RAD;
	
	m.X1 =  1.0 + (1.0 - cos(angle)) * (x * x - 1.0);
	m.Y1 = -z * sin(angle) + (1.0 - cos(angle)) * x * y;
	m.Z1 =  y * sin(angle) + (1.0 - cos(angle)) * x * z;
	m.X2 =  z * sin(angle) + (1.0 - cos(angle)) * x * y;
	m.Y2 =  1.0 + (1.0 - cos(angle)) * (y * y - 1.0);
	m.Z2 = -x * sin(angle) + (1.0 - cos(angle)) * y * z;
	m.X3 = -y * sin(angle) + (1.0 - cos(angle)) * x * z;
	m.Y3 =  x * sin(angle) + (1.0 - cos(angle)) * y * z;
	m.Z3 =  1.0 + (1.0 - cos(angle)) * (z * z - 1.0);
	
	return vec.apply_matrix(m);
}

void Ray::translate(GLfloat x, GLfloat y, GLfloat z)
{
	origin.x += x;
	origin.y += y;
	origin.z += z;
}

void Ray::rotate(GLfloat ang, GLfloat x, GLfloat y, GLfloat z)
{
	origin = rotate_vec3(ang, x, y, z, origin);
	direction = rotate_vec3(-ang, x, y, z, direction);
}

void Vec3::set_pos(GLfloat x1, GLfloat y1, GLfloat z1)
{
	x = x1;
	y = y1;
	z = z1;
}

void Vec3::set_posv(GLfloat pos[3])
{
	x = pos[X];
	y = pos[Y];
	z = pos[Z];
}

void Vec3::to_screen()
{
	glVertex3f(x, y, z);
}

void Vec3::set_screen_normal()
{
	glNormal3f(x, y, z);
}

void Vec3::operator=(GLfloat v)
{
	x = v;
	y = v;
	z = v;
}

Vec3 Vec3::operator+(Vec3 b)
{
	Vec3 a;
	a.x = x + b.x;
	a.y = y + b.y;
	a.z = z + b.z;
	return a;
}

Vec3 Vec3::operator-(Vec3 b)
{
	Vec3 a;
	a.x = x - b.x;
	a.y = y - b.y;
	a.z = z - b.z;
	return a;
}

Vec3 Vec3::operator/(Vec3 b)
{
	Vec3 a;
	a.x = x / b.x;
	a.y = y / b.y;
	a.z = z / b.z;
	return a;
}

Vec3 Vec3::operator/(GLfloat n)
{
	Vec3 a;
	a.x = x / n;
	a.y = y / n;
	a.z = z / n;
	return a;
}

Vec3 Vec3::apply_matrix(GLfloat mX1, GLfloat mX2, GLfloat mX3,
			GLfloat mY1, GLfloat mY2, GLfloat mY3,
			GLfloat mZ1, GLfloat mZ2, GLfloat mZ3)
{
	Vec3 ret;
 	
	ret.x = x * mX1 + y * mY1 + z * mZ1;
	ret.y = x * mX2 + y * mY2 + z * mZ2;
	ret.z =	x * mX3 + y * mY3 + z * mZ3;
	
	return ret;
}

Vec3 Vec3::apply_matrix(Matrix3x3 m)
{
	Vec3 ret;
 	
	ret.x = x * m.X1 + y * m.Y1 + z * m.Z1;
	ret.y = x * m.X2 + y * m.Y2 + z * m.Z2;
	ret.z =	x * m.X3 + y * m.Y3 + z * m.Z3;
	
	return ret;
}

Triangle::Triangle(Vec3 a, Vec3 b, Vec3 c)
{
	vx[0] = a;
	vx[1] = b;
	vx[2] = c;
}

Triangle::~Triangle()
{
}

Vec3 Triangle::normal()
{
	Vec3 n;
	Vec3 a, b;
	
	a.x = vx[1].x - vx[0].x; a.y = vx[1].y - vx[0].y; a.z = vx[1].z - vx[0].z;
	b.x = vx[2].x - vx[1].x; b.y = vx[2].y - vx[1].y; b.z = vx[2].z - vx[1].z;
	n = cross_product(a, b);
	return n;
}

// From:
//  http://www.lighthouse3d.com/opengl/maths/index.php?raytriint
bool Triangle::intersects_ray(Ray r)
{
	Vec3 	e1, e2, h, s, q;
	GLdouble a, f, u, v, t;

	e1 = vx[1] - vx[0];
	e2 = vx[2] - vx[0];
	h = cross_product(r.direction, e2);
	a = dot_product(e1, h);
	
	if(a > -0.0000001 && a < 0.0000001)
		return false;
	
	f = 1.0 / a;
	s = r.origin - vx[0];
	u = f * dot_product(s, h);
	
	if(u < 0.0 || u > 1.0)
		return false;

	q = cross_product(s, e1);
	v = f * dot_product(r.direction, q);
	if(v < 0.0 || u + v > 1.0)
		return false;
	
	t = f * dot_product(e2, q);
	if(t > 0.0000001)
		return true;

	return false;
}

Plane::Plane()
{
	distance = 0.0;
	nnormals = 0;
	nvertices = 0;
	nvertices_added = 0;
	normal.set_pos(0.0, 0.0, 1.0);
}

Plane::~Plane()
{
	for(int i = 0; i < nvertices_added; i++)
	{
		delete vertex[self_added_vertex[i]];
	}
}

GLfloat Plane::distance_to_ray(Ray r)
{
	Vec3 p2;
	
	p2.x = center.x - r.origin.x;
	p2.y = center.y - r.origin.y;
	p2.z = center.z - r.origin.z;
	
	return dot_product(normal, p2) / dot_product(r.direction, normal);
}

void Plane::add_normal(Vec3 *set_normal)
{
	if(nnormals < 4)
	{
		normal_v[nnormals] = set_normal;
		nnormals++;
	}
}

void Plane::add_vertex(Vec3 *set_vertex)
{
	if(nvertices < 4)
	{
		vertex[nvertices] = set_vertex;
		nvertices++;
	}
	if(nvertices > 2)
	{
		calculate_center();
		calculate_normal();
	}
}

void Plane::add_vertex(GLdouble x, GLdouble y, GLdouble z)
{
	if(nvertices < 4)
	{
		vertex[nvertices] = new Vec3;
		vertex[nvertices]->set_pos(x, y, z);
		self_added_vertex[nvertices_added++] = nvertices;
		nvertices++;
	}
	if(nvertices > 2)
	{
		calculate_center();
		calculate_normal();
	}
}

void Plane::to_screen()
{
	if(nvertices > 2)
	{
		glBegin(GL_POLYGON);
		
		normal.set_screen_normal();
		for(int i = 0; i < nvertices; i++)
		{
			if(i < nnormals)
				normal_v[i]->set_screen_normal();
			vertex[i]->to_screen();
		}
		glEnd();
	}
}

void Plane::to_screen_height()
{
	if(nvertices > 2)
	{
		glBegin(GL_POLYGON);
		
		normal.set_screen_normal();
		for(int i = 0; i < nvertices; i++)
		{
			if(i < nnormals)
				normal_v[i]->set_screen_normal();
			
			glColor3f(vertex[i]->z/ 25.6, 0.8, 0.0);
			vertex[i]->to_screen();
		}
		glEnd();
	}
}

void Plane::to_screen(int num_textures, GLfloat tex_coords[4][2])
{
	if(nvertices > 2)
	{
		glBegin(GL_POLYGON);

		normal.set_screen_normal();
		for(int i = 0; i < nvertices; i++)
		{
			set_tex_coords(num_textures, tex_coords, i);
			vertex[i]->to_screen();
		}
		
		glEnd();
	}
}

void Plane::calculate_normal()
{
	GLfloat vec[3];
	GLfloat v1x = vertex[1]->x - vertex[0]->x;
	GLfloat v1y = vertex[1]->y - vertex[0]->y;
	GLfloat v1z = vertex[1]->z - vertex[0]->z;

	GLfloat v2x = vertex[2]->x - vertex[0]->x;
	GLfloat v2y = vertex[2]->y - vertex[0]->y;
	GLfloat v2z = vertex[2]->z - vertex[0]->z;

//    The next step is the Cross product:
	vec[X] = (v1y * v2z) - (v1z * v2y);
	vec[Y] = (v1z * v2x) - (v1x * v2z);
	vec[Z] = (v1x * v2y) - (v1y * v2x);

	distance = sqrt(vec[X] * vec[X] + vec[Y] * vec[Y] + vec[Z] * vec[Z]);
	
	if(distance)
		normal.set_pos(vec[X] / distance, vec[Y] / distance, vec[Z] / distance);

	if(center.x < 0.0 && normal.x > 0.0)
		normal.x =  -normal.x;
	
	if(center.y < 0.0 && normal.y > 0.0)
		normal.y = -normal.y;
	
	if(center.z < 0.0 && normal.z > 0.0)
		normal.z = -normal.z;
	
	if(center.x > 0.0 && normal.x < 0.0)
		normal.x = -normal.x;
	
	if(center.y > 0.0 && normal.y < 0.0)
		normal.y = -normal.y;
	
	if(center.z > 0.0 && normal.z < 0.0)
		normal.z = -normal.z;
}

Vec3 Plane::calculate_center()
{
	center.set_pos(0.0, 0.0, 0.0);
	for(int i = 0; i < nvertices; i++)
		center.set_pos(	center.x + vertex[i]->x, 
				center.y + vertex[i]->y, 
				center.z + vertex[i]->z);
	
	center.set_pos(	center.x / nvertices, 
			center.y / nvertices,
			center.z / nvertices);
}	

void Plane::draw_normal()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(center.x,center.y,center.z);
		glVertex3f(center.x + normal.x* 0.5,
			   center.y + normal.y* 0.5,
			   center.z + normal.z* 0.5);
	glEnd();	
	glEnable(GL_LIGHTING);
}


void Plane::set_tex_coords(int num_textures, GLfloat tex_coords[4][2], int i)
{
	switch(num_textures)
	{
		case 1:	
			glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, tex_coords[i]);	
			break;
	
		case 2: 
			glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, tex_coords[i]);
			glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, tex_coords[i]);
			break;
						
		case 3: 	
			glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, tex_coords[i]);
			glMultiTexCoord2fvARB(GL_TEXTURE1_ARB, tex_coords[i]);
			glMultiTexCoord2fvARB(GL_TEXTURE2_ARB, tex_coords[i]);
			break;
						
		default: 	
			glMultiTexCoord2fvARB(GL_TEXTURE0_ARB, tex_coords[i]);
			break;
	}
}
