#ifndef ilist_h
#define ilist_h

#include "slist.h"
#include <iostream.h>

template <class T>
class ilist : private slist {
   public:
	ilist(void);
	~ilist(void);
	T& operator [] (unsigned int counter) const;
	void operator += (T element);
	void clear(void);
	int count(void) const;
	int Count(void) const;
	void sort(int (* compare_function) (const void *, const void *));
	T operator -= (unsigned int index);
	/*
	  void QSort(void);
	  void QSort(int, int);
	  */
   private:
	};


#endif
