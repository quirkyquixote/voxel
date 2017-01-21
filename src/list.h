/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_LIST_H_
#define SRC_LIST_H_

class Node {
 public:
	Node() : next(this), prev(this) { }
	Node(const Node &other) = delete;
	Node(Node &&other) = delete;
	virtual ~Node() { unlink(); }

	inline void link(Node *n)
	{
		Node *p = n->prev;
		this->next = n;
		this->prev = p;
		n->prev = this;
		p->next = this;
	}

	inline void unlink()
	{
		next->prev = prev;
		prev->next = next;
	}

	Node *next, *prev;
};

template<typename T> class List : public Node {
 public:
	class iterator {
	 public:
		iterator(Node *node) : node(node) { }
		~iterator() { }
		void operator++() { node = node->next; };
		void operator--() { node = node->prev; };
		bool operator!=(const iterator &rhs) const { return node != rhs.node; }
		T& operator*() { return *static_cast<T*>(node); }
	 private:
		Node *node;
	};

	List() : Node() { }
	List(const List &other) = delete;
	List(List &&other) = delete;
	~List() { }

	inline void append(Node *n)
	{
		n->link(this);
	}

	inline void prepend(Node *n)
	{
		n->link(this->next);
	}

	inline iterator begin() { return iterator(next); }
	inline iterator end() { return iterator(this); }
	inline iterator rbegin() { return iterator(prev); }
	inline iterator rend() { return iterator(this); }
};

#endif  // SRC_LIST_H_
