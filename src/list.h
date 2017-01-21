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
		iterator(Node *node) : node(node), next(node->next) { }
		~iterator() { }
		void operator++() { node = next; next = node->next; };
		bool operator!=(const iterator &rhs) const { return node != rhs.node; }
		T& operator*() { return *static_cast<T*>(node); }
	 private:
		Node *node;
		Node *next;
	};

	List() : Node() { }
	List(const List &other) = delete;
	List(List &&other) = delete;
	~List() { }

	inline void push_back(Node *n)
	{
		n->link(this);
	}

	inline void push_front(Node *n)
	{
		n->link(this->next);
	}

	inline iterator begin() { return iterator(next); }
	inline iterator end() { return iterator(this); }
};

#endif  // SRC_LIST_H_
