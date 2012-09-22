#ifndef __GRID_HPP
#define __GRID_HPP

#define GRID_WIDTH		256
#define GRID_HEIGHT		256
#define BIG_GRID_DIVISOR	64
#define BIG_GRID_TBOUND		12.0
#define HUGE_GRID_DIVISOR	16

#define S		2
#define F		3
#define	N		1

#define DIA_WEIGHT	14
#define WEIGHT		10

class GridSquare
{
	public:
		GridSquare()
			{ f = g = h = 65536; parent = -1; slope = 0; }
		GridSquare(int set_x, int set_y, int set_traversability)
			{ x = set_x; y = set_y; traversability = set_traversability; f = g = h = 65536; parent = -1; slope = 0; }
		~GridSquare()
			{ ; }

		void 	set_f(int i)			{ f = i; }
		void 	set_g(int i)			{ g = i; }
		void 	set_h(int i)			{ h = i; }
		void	set_parent(int i)		{ parent = i; }
		void	set_slope(int i)		{ slope = i; }		
		void	set_traversability(int i)	{ traversability = i; }
		
		int	get_slope()		{ return slope; }
		int	get_x()			{ return x; }
		int	get_y()			{ return y; }
		int	get_f()			{ return f; }
		int	get_g()			{ return g; }
		int	get_h()			{ return h; }
		int	get_parent()		{ return parent; }	
		bool	is_traversable()	{ return (bool) traversability; }

	private:
		int	slope;
		int	traversability;
		int	f, g, h, x, y, parent;
};

class Grid
{
	public:
		Grid();
		~Grid();
		
		void 		load(int grid[GRID_HEIGHT][GRID_WIDTH]);
		void		load_map(char *filename);
		void		load_heightmap(char *filename);
		void		highlight(int x, int y, GLfloat t);
		void 		to_screen();
		void		show_path(List *small_path, List *big_path, List *huge_path);

		int		nearest_traversable(int target, int range);
		void		get_path(int x0, int y0, int x1, int y1, List *path);
		void		get_big_path(int x0, int y0, int x1, int y1, List *path);
		void		get_huge_path(int x0, int y0, int x1, int y1, List *path);
		Plane		get_plane(GLfloat x, GLfloat y);
		bool		is_traversable(int x, int y);		
		
	private:
		GridSquare	huge_squares[HUGE_GRID_DIVISOR*HUGE_GRID_DIVISOR];
		GridSquare	big_squares[BIG_GRID_DIVISOR*BIG_GRID_DIVISOR];
		GridSquare	small_squares[GRID_HEIGHT*GRID_WIDTH];
		Plane		landscape[GRID_WIDTH][GRID_HEIGHT];
		Ray		vertices[GRID_WIDTH+1][GRID_HEIGHT+1];
		bool		has_heightmap;
};

int 	difference(int a, int b);
int 	distance_heuristic(int x, int y, int destx, int desty);
void	insert_sorted(List *l, int id, int score, GridSquare gs[]);
bool	find_path(GridSquare gs[GRID_HEIGHT*GRID_WIDTH], int start, int finish, List *path, int max_width, int max_height);

#endif	/* __GRID_HPP */
