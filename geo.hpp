#ifndef __GEO_HPP
#define __GEO_HPP

#define Point	Vec3
#define Vertex	Vec3

#define X 	0
#define Y 	1
#define Z 	2

#define PI 	3.141593
#define DEG2RAD 0.017453

struct Matrix3x3
{
	GLdouble	X1;
	GLdouble	X2;
	GLdouble	X3;
	
	GLdouble	Y1;
	GLdouble	Y2;
	GLdouble	Y3;

	GLdouble	Z1;
	GLdouble	Z2;
	GLdouble	Z3;
};

struct Vec3
{
	GLdouble	x;
	GLdouble	y;
	GLdouble	z;

	void		set_posv(GLfloat pos[3]);
	void		set_pos(GLfloat x1, GLfloat y1, GLfloat z1);
	void 		to_screen();		
	void 		set_screen_normal();
	void		operator=(GLfloat v);
	Vec3		operator-(Vec3 v);
	Vec3		operator+(Vec3 v);
	Vec3		operator/(Vec3 v);
	Vec3		operator/(GLfloat n);
	
	Vec3 		apply_matrix(Matrix3x3 m);
	Vec3 		apply_matrix(	GLfloat mX1, GLfloat mX2, GLfloat mX3,
					GLfloat mY1, GLfloat mY2, GLfloat mY3,
			  		GLfloat mZ1, GLfloat mZ2, GLfloat mZ3);
};

struct Ray
{
	Vec3 	origin;
	Vec3	direction;

	void	translate(GLfloat x, GLfloat y, GLfloat z);
	void	rotate(GLfloat ang, GLfloat x, GLfloat y, GLfloat z);
};

class Triangle
{
	public:
		Triangle(Vec3 a, Vec3 b, Vec3 c);
		~Triangle();
		bool	intersects_ray(Ray r);
		Vec3	normal();
	private:
		Vec3	vx[3];
};

class Plane
{
	public:
		Plane();
		~Plane();
		
		void 	add_vertex(GLdouble x, GLdouble y, GLdouble z);
		void	add_vertex(Vec3	*set_vertex);
		void	add_normal(Vec3 *set_normal);
		void 	to_screen();
		void 	to_screen_height();
		void 	to_screen(int num_textures, GLfloat tex_coords[4][2]);
		void 	draw_normal();

		Vec3	get_normal()	{ return normal; }
		GLfloat	distance_to_ray(Ray r);
		
	private:
		void 	set_tex_coords(int num_textures, GLfloat tex_coords[4][2], int i);
		void	calculate_normal();
		Vec3 	calculate_center();

		int	nnormals;
		int 	nvertices;
		int	self_added_vertex[4];
		int	nvertices_added;
		Vec3	*vertex[4];
		Vec3	*normal_v[4];
		Vec3	normal;
		Vec3	center;
		GLfloat distance;
		
};

GLdouble 	dot_product(Vec3 a, Vec3 b);
Vec3		cross_product(Vec3 a, Vec3 b);

#endif /* __PLANE_HPP */
