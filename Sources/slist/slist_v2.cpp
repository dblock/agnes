#include <agnespch.hpp>

#ifndef SLIST_V2_H
#define SLIST_V2_H

template <class T>
ilist<T>::~ilist(void){
  
}

template <class T>
ilist<T>::ilist(void){
  if ((sizeof(T))!=sizeof(void *)) {
  exit(0);
  }
}

template <class T>
T& ilist<T>::operator[](unsigned int counter) const{
  return((T&) slist::element(counter));
}

template <class T>
T ilist<T>::operator-=(unsigned int index){
  return((T) slist::remove(index));
}

template <class T>
void ilist<T>::operator+=(T element){
  slist::append(element);
}

template <class T>
void ilist<T>::clear(void){
  slist::clear();
}

template <class T>
int ilist<T>::count(void) const {
  return(slist::count());
}

template <class T>
int ilist<T>::Count(void) const {
  return slist::count();
}

template <class T>
void ilist<T>::sort(int (* compare_function) (const void *, const void *)){
  slist::sort(compare_function);
}

/*
  template <class T>
  void ilist<T>::QSort(void){  
  if (slist::count()) QSort(0, slist::count() - 1);
  }
  
  template <class T>
  void ilist<T>::QSort(int l, int r){  
  int i = l;
  int j = r;
  T pivot = * (T *) slist::elements_structure[(l+r)/2]; 
  while (i<=j) {
  while(* (* (T *) slist::elements_structure[i])<(* pivot)) i++;
  while(* (* (T *) slist::elements_structure[j])>(* pivot)) j--;
  if (i<=j){
  T * t = (T * ) slist::elements_structure[i];
  slist::elements_structure[i] = slist::elements_structure[j];
  slist::elements_structure[j] = t;
  i++;
  j--;
  }
  }
  if (l < j) QSort(l, j);
  if (l < r) QSort(i, r);
  }
  */

#endif


