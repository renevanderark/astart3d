#ifndef __DATASTRUCT_HPP
#define __DATASTRUCT_HPP

#define MAX_BRANCHES 	8

struct Node 
{
	int	n;
	Node	*next;
};

class List
{
	public:
		List();
		~List();

		int	operator[](int i);
		int	index(int i);
		int	pop();
		void	insert(int value, int position);	
		void 	insert(int value);
		void 	print();
		bool 	empty()	{ if(first == 0) return true; else return false; }
		
		bool	contains(int value);
		void	remove(int value);
		void	move_node_from(int value, List *l);
		
		void	copy(List l);

		int	walkthrough();
		void	set_active_to_first()	{ active = first; }
		int	top()			{ return first->n; }
		int	size()			{ return list_size; }
		void	destroy()		{ if(first != 0) destroy(first); list_size = 0; first = 0; }
	private:
		void	destroy(Node *current);
		void	print(Node *current);
		int	get_location(Node *current, int pos, int i);
		Node	*remove(Node *current, int value);
		Node	*insert(Node *current, int value);
		Node	*insert(Node *current, int value, int at_pos);
		Node 	*first;
		Node	*active;

		int 	list_size;
};

class Tree
{
	public:
		Tree(Tree *set_parent, bool set_success);
		~Tree();
		
		
		void	insert_leaf(int val);
		Tree	*append_child(bool set_success);
		Tree	*append_sibling(bool set_success);
		void	print();
		void	set_success(bool s)	{ success = s; }
		bool	succeeds()		{ return success; }
		Tree	*get_child(int which)	{ if(which < nChildren) return child[which]; }
		Tree	*get_parent()		{ return parent; }
			
	private:
		void	print(int depth);
		int	nChildren;
		bool	success;

		List	leaves;
		Tree	*child[MAX_BRANCHES];
		Tree	*parent;
};

class RequestQueue
{
	public:
		RequestQueue();
		~RequestQueue();

		void 	push(int value);
		void	show()			{ queue.print(); }
		int	pop();
		int	top();
	
	private:
		List	queue;
};

#endif /* __DATASTRUCT_HPP */
