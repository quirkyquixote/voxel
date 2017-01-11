
#ifndef VOXEL_LIST_H_
#define VOXEL_LIST_H_

struct list {
	struct list *next, *prev;
};

static inline void list_init(struct list *l)
{
	l->next = l;
	l->prev = l;
}

static inline int list_empty(struct list *l)
{
	return l->next == l;
}

static inline void list_link(struct list *l, struct list *x)
{
	x->prev = l->prev;
	x->next = l;
	x->prev->next = x;
	x->next->prev = x;
}

static inline void list_unlink(struct list *x)
{
	x->prev->next = x->next;
	x->next->prev = x->prev;
}

static inline void list_append(struct list *l, struct list *x)
{
	list_link(l, x);
}

static inline void list_prepend(struct list *l, struct list *x)
{
	list_link(l->next, x);
}

static inline void list_merge(struct list *l1, struct list *l2)
{
	if (list_empty(l2))
		return;
	l1->prev->next = l2->next;
	l2->next->prev = l1->prev;
	l2->prev->next = l1;
	l1->prev = l2->prev;
	list_init(l2);
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
	for (struct list *_iter = (_list)->next, *_next; \
		_iter != (_list) && (_next = _iter->next) \
			&& (_elem = list_entry(_iter,typeof(*_elem),_memb)); \
		_iter = _next)

#endif
