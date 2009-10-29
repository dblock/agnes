#ifndef slist_h
#define slist_h

class slist {
protected:
	unsigned int elements_count;
	void ** elements_structure;
	~slist(void);
	void append(void * element);
	void * remove(unsigned int index);
	void * & element(unsigned int index) const;
	void insert (void * element, unsigned int index);
public:
	slist(void);
	unsigned int count(void) const;
	void operator+=(void * element) {append(element);}
	void operator-=(unsigned int index) {remove(index);}
	void * & operator[](unsigned int index) const;
	void clear(void);
	void sort(int (* compare_function) (const void *, const void *));
	};

#endif
