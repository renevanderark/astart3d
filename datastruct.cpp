#include <iostream>
#include "datastruct.hpp"

using namespace std;

List::List()
{
	first = 0;
	active = 0;
	list_size = 0;
}

List::~List()
{
	list_size = 0;
	if(first != 0)
		destroy(first);
}

void List::copy(List l)
{
	if(first != 0)
		destroy(first);

	for(int i = l.size() - 1; i > -1; i--)
		insert(l[i]);

	list_size = l.size();
}

int List::get_location(Node *current, int pos, int i)
{
	if(current == 0)
		return -1;
	
	if(pos == i)
		return current->n;
	
	return get_location(current->next, pos + 1, i);
}

int List::index(int i)
{
	if(i < list_size)
		return get_location(first, 0, i);
	else
		return -1;
}

int List::operator[](int i)
{
	if(i < list_size)
		return get_location(first, 0, i);
	else
		return -1;
}

Node *List::insert(Node *current, int value)
{
	Node *new_node = new Node;
 
	new_node->n = value;
	new_node->next = current;
 	
	return new_node;
}

void List::destroy(Node *current)
{
	if(current != 0)
	{
		Node *tmp = current->next;
		delete current;
		destroy(tmp);
	}
}

void List::print(Node *current)
{
	if(current != 0)
	{
		cout << current->n << ",";
		print(current->next);
	} else
		cout << endl;
}

void List::insert(int value)
{
	first = insert(first, value);
	active = first;
	list_size++;
}

void List::print()
{
	print(first);
}

Node *List::insert(Node	*list, int value, int at_pos)
{
	Node *new_node = new Node;
	Node *cur;
	Node *prev;
	int i;
	for(	cur = list, prev = 0, i = 0; 
		cur != 0 && i != at_pos; 
		prev = cur, cur = cur->next, i++)
		;	
	
	new_node->n = value;
	
	if(cur == 0)
	{
		new_node->next = 0;
		prev->next = new_node;
		return list;
	}
	
	if(prev == 0)
	{
		new_node->next = list;
		return new_node;
	}
	
	prev->next = new_node;
	new_node->next = cur;
	
	return list;
}

void List::insert(int value, int position)
{
	first = insert(first, value, position);
	active = first;
	list_size++;
}

Node *List::remove(Node	*list, int value)
{
	Node *cur;
	Node *prev;
	for(	cur = list, prev = 0; 
		cur != 0 && cur->n != value; 
		prev = cur, cur = cur->next)
		;	
	
	if(cur == 0)
		return list;
	
	if(prev == 0)
		list = list->next;
	else
		prev->next = cur->next;	
	
	list_size--;
	delete cur;
	return list;
}

void List::remove(int value)
{
	first = remove(first, value);
}

bool List::contains(int value)
{
	if(first == 0)
		return false;
	
	for(Node *cur = first; cur != 0; cur = cur->next)
	{
		if(cur->n == value)
			return true;
	}
	return false;		
}

int List::walkthrough()
{
	if(active != NULL)
	{
		int x = active->n;
		active = active->next;
		return x;
	} 

	return -1;	
}

void List::move_node_from(int value, List *l)
{
	insert(value);
	l->remove(value);
}

Tree::Tree(Tree *set_parent, bool set_success)
{
	nChildren = 0;
	success = set_success;
	parent = set_parent;
}

Tree::~Tree()
{
	for(int i = 0; i < nChildren; i++)
		delete child[i];
	
}

void Tree::insert_leaf(int val)
{
	leaves.insert(val);
}

Tree *Tree::append_child(bool set_success)
{
	if(nChildren < MAX_BRANCHES)
		child[nChildren++] = new Tree(this, set_success);

	return child[nChildren-1];
}

Tree *Tree::append_sibling(bool set_success)
{
	return parent->append_child(set_success);
}

void Tree::print()
{
	cout << "0";
	
	if(!success)
		cout << ":-1";

	if(!leaves.empty())
	{
		cout << ":";
		leaves.print();
	} else
		cout << endl;

	for(int i = 0; i < nChildren; i++)
			child[i]->print(1);
}

void Tree::print(int depth)
{
	for(int i = 0; i < depth; i++)
		cout << " ";

	cout << depth;

	if(!success)
		cout << ":-1";

	if(!leaves.empty())
	{
		cout << ":";
		leaves.print();
	} else
		cout << endl;

	for(int i = 0; i < nChildren; i++)
		child[i]->print(depth + 1);
}

RequestQueue::RequestQueue()
{
}

RequestQueue::~RequestQueue()
{
}

void RequestQueue::push(int value)
{
	if(queue.size() > 0)
		queue.insert(value, queue.size());
	else
		queue.insert(value);
}

int List::pop()
{
	int x = -1;
	if(list_size > 0)
	{
		x =  top();
		remove(x);
	}
	return x;
}

int RequestQueue::pop()
{
	int x = -1;
	if(queue.size() > 0)
	{
		x = queue.top();
		queue.remove(x);
	}
	return x;
}

int RequestQueue::top()
{
	if(queue.size() > 0)
		return queue.top();
	return -1;
}
