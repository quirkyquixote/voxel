
#ifndef VOXEL_LIST_H_
#define VOXEL_LIST_H_

struct List {
	inline List() : next(this), prev(this) { }

	virtual ~List()
	{
		prev->next = next;
		next->prev = prev;
	}

	List *next, *prev;
};

inline bool empty(const List *l)
{
	return l->next == l;
}

inline void link(List *lhs, List *rhs)
{
	rhs->prev = lhs->prev;
	rhs->next = lhs;
	rhs->prev->next = rhs;
	rhs->next->prev = rhs;
}

inline void unlink(List *l)
{
	l->prev->next = l->next;
	l->next->prev = l->prev;
}

inline void append(List *lhs, List *rhs)
{
	link(lhs, rhs);
}

inline void prepend(List *lhs, List *rhs)
{
	link(lhs->next, rhs);
}

inline void merge(List *lhs, List *rhs)
{
	if (empty(rhs))
		return;
	lhs->prev->next = rhs->next;
	rhs->next->prev = lhs->prev;
	rhs->prev->next = lhs;
	lhs->prev = rhs->prev;
	rhs->next = rhs;
	rhs->prev = rhs;
}

#define list_entry(l,type,member) \
	((type *)((char *)(l) - (unsigned long)(&((type *)0)->member)))

#define list_head(l,type,member) \
	list_entry((l)->next,type,member)

#define list_tail(l,type,member) \
	list_entry((l)->prev,type,member)

#define list_next(x,member) \
	list_entry((x)->member.next,typeof(*x),member)

#define list_foreach(pos,list,member) \
	for (pos = list_head(list, typeof(*pos), member); \
			&pos->member != (list); \
			pos = list_next(pos, member))

#define list_foreach_safe(_elem,_list,_memb) \
	for (List *_iter = (_list)->next, *_next; \
		_iter != (_list) && (_next = _iter->next) \
			&& (_elem = list_entry(_iter,typeof(*_elem),_memb)); \
		_iter = _next)

#endif
