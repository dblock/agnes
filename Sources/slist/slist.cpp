#include <agnespch.hpp>

void slist::sort(int (* compare_function) (const void *, const void *)){
	/* compare function should look like this:
		int cmp(const void *, const void *)
		element is accessed via: (* (typecase **)
	*/
	qsort(elements_structure, elements_count, sizeof(void *), compare_function);
	}

void * slist::remove(unsigned int index){
	void * returnable = element(index);
	if (index < elements_count){
		elements_count --;
		for (unsigned int k = index;k<(unsigned int) elements_count;k++) 
			elements_structure[k] = elements_structure[k+1];
		}
	return(returnable);
	}

slist::slist(void){
	elements_count = 0;
	elements_structure = 0;
	}

slist::~slist(void){
	}

void slist::insert(void * element, unsigned int index){
	if (index>=elements_count) append(element);
	else {
		elements_structure = (void **) realloc(elements_structure, sizeof(void *)*(++elements_count));
		for (unsigned int i=elements_count-1;i>=index;i--)
			elements_structure[i]=elements_structure[i-1];
		elements_structure[index] = element;
		}
	}

void slist::append(void * element){
	elements_structure = (void **) realloc(elements_structure, sizeof(void *)*(++elements_count));
	elements_structure[elements_count - 1] = element;
	}

void slist::clear(void){
	if (elements_count) {
		free(elements_structure);
		elements_count = 0;
		}
	elements_structure = 0;
	}

void * & slist::operator[](unsigned int index) const {
	return(element(index));
	}

void * & slist::element(unsigned int index) const {
	if (index < elements_count)
		return(elements_structure[index]);
		else {
		cout << "slist::element:invalid access to element [" << index << "] returning last pointer" << elf;
		return(elements_structure[elements_count - 1]);
		}
	}

unsigned int slist::count(void) const {
	return(elements_count);
	}

