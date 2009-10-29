#include <agnespch.hpp>

#ifndef C_HASHTABLE_CPP
#define C_HASHTABLE_CPP

template <class V>
void CHashTable<V>::DelHash(int Index){  
  if (Index < size_) {
    nElements_--;
    Karray_[Index].Free();
    Varray_[Index] = Vzero_;  
  }
}

template <class V>
void CHashTable<V>::Del(int Index){
  int counter = 0;
  for (int i=0;i<size_;i++) {
    if (Karray_[i].StrLength()) {
      if (counter == Index) {
	nElements_--;
	Karray_[i].Free();
	Varray_[i] = Vzero_;
	return;
      };
      counter++;
    }
  }
}

template <class V>
void CHashTable<V>::Del(const CString& key){
  if (key.StrLength()) {
    if ((cached_ndx>=0)&&(Karray_[cached_ndx] == key)) { 
      nElements_--;
      Karray_[cached_ndx].Free();
      Varray_[cached_ndx] = Vzero_;
      cached_ndx = -1;
      return;
    }
    int counter = 0;
    while(counter < size_) {
      this->ndx = Hash(key, counter);
      /*
	if (!(Karray_[ndx].StrLength())) {
	nElements_--;
	return;
	}
      */
      if (Karray_[ndx] == key) {
	counter++;
	nElements_--;
	Karray_[ndx].Free();
	Varray_[ndx] = Vzero_;
	return;
      }
      else counter++;
    }
  }
}

template <class V>
V * CHashTable<V>::Set(const CString& key, const V& value, int warn, int fail){
  if (key.StrLength()) {

    if ((cached_ndx>=0)&&(Karray_[cached_ndx] == key)) { 
      Varray_[cached_ndx] = value;
      return &Varray_[cached_ndx];
    }

    int counter = 0;
    while(counter < size_) {
      this->ndx = Hash(key, counter);
      if (!(Karray_[ndx].StrLength())) {
	if (fail) return 0;
	//if (warn) cgiOut << "CHashTable::set() - entry does not exist! [" << key << "]" << elf;
	return Add(key, value);
      }
      if (Karray_[ndx] == key) {
	counter++;
	cached_ndx = ndx;
	Varray_[ndx] = value;
	return &Varray_[ndx];
      }
      else counter++;
    }
    return Add(key, value);
  } else return 0;
}

template <class V>
V * CHashTable<V>::Add(const CString& key, const V& value) {
  if (key.StrLength()) {
    int counter = 0;
    if (LoadFactor() > .90) Grow();
    while (counter < size_) {
	ndx = Hash(key, counter);
	if (Karray_[ndx].StrLength() == 0) {
	    Karray_[ndx] = key;
	    Varray_[ndx] = value;
	    nElements_++;
	    cached_ndx = ndx;
	    return &Varray_[ndx];
	  } else counter++;
    }
    /*
      overflow, grow again (unstable) 
      */
    Grow();
    return Add(key, value);    
  } else return 0;
}

template <class V>
CHashTable<V>::~CHashTable(void){
  delete [] Karray_; 
  if (Varray_) delete [] Varray_; 
}

template <class V>
V CHashTable<V>::operator[](int Index) const {
  return Get(Index);
}

template <class V>
CString CHashTable<V>::GetKey(int Index) const {
  int counter = 0;
  for (int i=0;i<size_;i++) {
    if (Karray_[i].StrLength()) {
      if (counter == Index) return Karray_[i];
      counter++;
    }
  }
  return "";
}

template <class V>
V CHashTable<V>::Get(int Index) const {
  int counter = 0;
  for (int i=0;i<size_;i++) {
    if (Karray_[i].StrLength()) {
      if (counter == Index) return Varray_[i];
      counter++;
    }
  }
  return Vzero_;
}

template <class V>
CHashTable<V>::CHashTable(const CHashTable<V>& Source){
  Clone(Source);
}

template <class V>
CHashTable<V>& CHashTable<V>::operator=(const CHashTable<V>& Source){
  return Clone(Source);
}

template <class V>
CHashTable<V>& CHashTable<V>::Clone(const CHashTable<V>& Source){
  Clear();
  for (int i=0;i<Source.size_;i++){
    Add(Source.Karray_[i], Source.Varray_[i]);
  }
  return (* this);
}

template <class V>
int CHashTable<V>::Contains(const CString& key) const {
  if (key.StrLength()) {
    
    if ((cached_ndx>=0)&&(Karray_[cached_ndx] == key)) { 
      return cached_ndx;
    }
    int counter = 0;
    while(counter < size_) {
      ((CHashTable<V> *) this)->ndx = Hash(key, counter);
      if (Karray_[ndx] == key) {
	counter++;
	((CHashTable<V> *) this)->cached_ndx = ndx;
	return ndx;
      }
      else counter++;
    }
  }
  return -1;
}

template <class V>
V CHashTable<V>::Search(const CString& key) const {
  if (key.StrLength()) {
    if ((cached_ndx>=0)&&(Karray_[cached_ndx] == key)) { 
      return Varray_[cached_ndx];
    }
    int counter = 0;
    while(counter < size_) {
      ((CHashTable<V> *) this)->ndx = Hash(key, counter);
      if (!(Karray_[ndx].StrLength())) {
	return Vzero_;
      }
      if (Karray_[ndx] == key) {
	counter++;
	((CHashTable<V> *) this)->cached_ndx = ndx;
	return Varray_[ndx];
      }
      else counter++;
    }
  }
  return Vzero_;
}

template <class V>
V * CHashTable<V>::GetRef(int Index) {
  int counter = 0;
  for (int i=0;i<size_;i++) {
    if (Karray_[i].StrLength()) {
      if (counter == Index) return & Varray_[i];
      counter++;
    }
  }
  return & Vzero_;
}

template <class V>
V * CHashTable<V>::SearchRef(const CString& key) const {
  if (key.StrLength()) {
    int counter = 0;
    if ((cached_ndx>=0)&&(Karray_[cached_ndx] == key)) { 
      return &Varray_[cached_ndx];
    }
    while(counter < size_) {
      ((CHashTable<V> *) this)->ndx = Hash(key, counter);
      if (!(Karray_[ndx].StrLength())) return NULL;
      if (Karray_[ndx] == key) {
	counter++;
	((CHashTable<V> *) this)->cached_ndx = ndx;
	return &Varray_[ndx];
      }
      else counter++;
    }
  }
  return NULL;
}

template <class V>
int CHashTable<V>::Hash(const CString& ckey, int counter) const {
  CString key(ckey);key.StrTrim32();key.LowerCase();
  unsigned long keyval = 0;
  unsigned long k2;
  int len = key.StrLength();
  if (len < 4) strcpy((char *) &keyval, key.asString());
  if (len >= 4) { memcpy(&keyval, key.asString(), 4); }
  if (len >= 8) { memcpy(&k2, key.asString()+4, 4); keyval ^= k2; }
  if (len >= 12) { memcpy(&k2, key.asString()+8, 4); keyval ^= k2; }
  keyval = abs(keyval);
  double v1 = keyval * 0.6180339887;  // (sqrt5-1)/2, Knuth
  double hv1 = (v1 - (int) v1);
  int h1 = (int) (size_ * hv1);
  double v2 = keyval * 0.87380339887;  // double hash, me 
  double hv2 = (v2 - (int) v2);      
  int h2 = ((int) (size_ * hv2)) * 2 + 1; 
  int retval = (h1 + counter*h2) % size_; 
  return retval;
}

template <class V>
int CHashTable<V>::Grow(int new_size) {
  cached_ndx = -1;  
  if (new_size <= size_)
    new_size = size_ + (((int)(size_*0.1))+1);
  CString * newKarray = new CString[new_size];
  if (!newKarray) return -1;
  V * newVarray = new V[new_size];

  if (!newVarray) { delete [] newKarray; return -1; }

  for (int i = 0; i < new_size; i++) newVarray[i] = Vzero_;
  int oldSize = size_;

  size_ = new_size;
  nElements_ = 0;
  CString * oldKarray = Karray_;
  V * oldVarray = Varray_;

  Karray_ = newKarray;
  Varray_ = newVarray;
  
  for (int ji = 0; ji < oldSize; ji++)  
    if (oldKarray[ji].StrLength())
      Add(oldKarray[ji], oldVarray[ji]);
     
  delete [] oldKarray;
  delete [] oldVarray;
  return 0;
}

template <class V>
void CHashTable<V>::Clear() {
  cached_ndx = -1;
  for (int i = 0; i < size_; i++){
    if (Karray_) {
      Karray_[i].Free();
      Varray_[i] = Vzero_;
    }
  }
  nElements_ = 0;
}

/* returns a power of two number equal to or larger than val */
template <class V>
int CHashTable<V>::GPow2(int val){
  int p = 1;
  int v = val;
  while (v >>= 1) p <<=1;
  return (p == val) ? p : p << 1;
}

template <class V>
float CHashTable<V>::LoadFactor(void) const {
  return nElements_ / (float) size_; 
}

template <class V>
int CHashTable<V>::Count(void) const{
  return nElements_; 
}

/* number of available entries the array currently supports */
template <class V>
int CHashTable<V>::Size(void) const {
  return size_;
}

template <class V>
V * CHashTable<V>::Set(const CString& key, const V& value) {
  return Set(key, value, 0, 0); 
}

template <class V>
CHashTable<V>::CHashTable(V zero) {
  Vzero_ = zero;
  nElements_ = 0;
  size_ = GPow2(10);
  Karray_ = new CString[size_];
  V * newVarray = new V[size_];
  Varray_ = newVarray;
  int l_cached_ndx = -1;
  cached_ndx = l_cached_ndx;
}


#endif
