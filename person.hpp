#ifndef __PERSON_HPP
#define __PERSON_HPP

#define	NO_REQUEST	0
#define REQUEST_PATH	1
#define MAX_PERSONS	1024

#define N_SKILLS2	1
#define	FISHING		0

#define N_SKILLS1	2
#define STRENGTH	0
#define INTELLIGENCE	1

struct Skills
{
	int 	primary[N_SKILLS1];
	int	secondary[N_SKILLS2];
	
	float	talent[N_SKILLS2];
	float	affinity[N_SKILLS2];
	float	basic_talent[N_SKILLS1];
};

class Activity
{
	public:
		Activity(int set_type);
		~Activity();

		void 	find_workspace(Grid *g);
		void	go_home(Grid *g);
		
	private:
		int	train_skills1[N_SKILLS1];
		int	type;
};

class PersonBase
{
	public:
		PersonBase();
		~PersonBase();
		
		GLfloat x()				{ return xPos; }
		GLfloat	y()				{ return yPos; }
		
		List	*get_small_path()		{ return &small_path; }
		List	*get_big_path()			{ return &big_path; }
		List	*get_huge_path()		{ return &huge_path; }
		
		void	to_screen(Plane on_plane);
		int	walk(Grid *g);
		void	set_pos(GLfloat xx, GLfloat yy)	{ small_target_x = xPos = xx; small_target_y = yPos = yy; }

		void	start_walking()			{ walking = true; }
		void	stop_walking()			{ walking = false; }
		bool	is_walking()			{ return walking; }
	
		void	find_path_to(int x, int y, Grid *g);

		void	find_small_path(Grid *g);
		void	find_big_path(int x, int y, Grid *g);
		void	find_huge_path(int x, int y, Grid *g);

	protected:
		List	small_path;
		List	big_path;
		List	huge_path;
		
		GLfloat xPos;
		GLfloat yPos;
		
		GLfloat	small_target_x;
		GLfloat small_target_y;
		GLfloat big_target_x;
		GLfloat big_target_y;
		
		int	real_target_x;	
		int	real_target_y;
		
		int	xPath_pending;
		int	yPath_pending;
		
		bool	walking;
};

#endif /* __PERSON_HPP */
