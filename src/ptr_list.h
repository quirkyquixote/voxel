/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_PTR_LIST_H_
#define SRC_PTR_LIST_H_

class PtrNode {
 public:
	PtrNode() : next(this), prev(this) { }
	PtrNode(const PtrNode &other) = delete;
	PtrNode(PtrNode &&other) = delete;
	virtual ~PtrNode() { unlink(); }

	inline void link(PtrNode *n)
	{
		PtrNode *p = n->prev;
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

	PtrNode *next, *prev;
};

template<typename T> class PtrList : public PtrNode {
 public:
	class iterator {
	 public:
		explicit iterator(PtrNode *node) : node(node), next(node->next) { }
		~iterator() { }
		void operator++() { node = next; next = node->next; }
		bool operator!=(const iterator &rhs) const { return node != rhs.node; }
		T* operator*() { return static_cast<T*>(node); }
	 private:
		PtrNode *node;
		PtrNode *next;
	};

	PtrList() : PtrNode() { }
	PtrList(const PtrList &other) = delete;
	PtrList(PtrList &&other) = delete;
	~PtrList() { }

	inline void push_back(PtrNode *n)
	{
		n->link(this);
	}

	inline void push_front(PtrNode *n)
	{
		n->link(this->next);
	}

	inline iterator begin() { return iterator(next); }
	inline iterator end() { return iterator(this); }
};

#endif  // SRC_PTR_LIST_H_
