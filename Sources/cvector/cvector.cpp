#include <agnespch.hpp>

#ifndef CVECTOR_CPP_INCL
#define CVECTOR_CPP_INCL

//#define _DEBUG_CPP

static const float XFactor = (float) 0.1;
static const int MaxFactor = 1000;
#define _MIN(a,b) ((a)>(b)?(b):(a))

/*
  ICVector
  */
template <class T>
void ICVector<T>::Resize(void) {
  if (m_nArrayFill * 2 < m_nArrayDim) {
    m_nArrayDim = m_nArrayFill;
    T* m_pArray_new = new T[m_nArrayDim];
    for (int i=0;i<m_nArrayFill;i++)
      m_pArray_new[i]=m_pArray[i];
    delete[] m_pArray;
    m_pArray = m_pArray_new;    
  }
}

template <class T>
ICVector<T>::ICVector(int iSize) {
  AllocSize(iSize);
}

template<class T>
void ICVector<T>::AllocSize(int iSize){
#ifdef _DEBUG_CPP
  assert(iSize > 0);
#endif  
  m_nArrayDim = iSize;
  m_nArrayFill = 0;
  T * m_pArray_local = new T[m_nArrayDim]; 
  m_pArray = m_pArray_local;
#ifdef _DEBUG_CPP
  assert(m_pArray);
#endif
  m_Refs = 1;
}

template <class T>
ICVector<T>::ICVector(void) {  
  AllocSize(5);
}

template <class T>
ICVector<T>::~ICVector(void){ 
  delete[] m_pArray;
}


template <class T>
ICVector<T> * ICVector<T>::ICVectorCopy(void){
  ICVector<T> * newICVector = new ICVector<T>(m_nArrayDim);
  for (int i=0;i<m_nArrayFill;i++) newICVector->AddElt(m_pArray[i]);  
  UnReg();
  return newICVector;
}

template<class T>
void ICVector<T>::QSortUnique(void){
  if (m_nArrayFill) {
    QSort(0, m_nArrayFill - 1);
    int j = 0;
    for (int i=1;i<m_nArrayFill;i++) 
      if (m_pArray[i] != m_pArray[j]) {
	j++; if (j!=i) m_pArray[j] = m_pArray[i];
      }
    m_nArrayFill = j+1;
  }
}

template <class T>
void ICVector<T>::QSort(int l, int r){   
  int i = l;
  int j = r;
  T pivot = m_pArray[(l+r)/2]; 
  while (i<=j) {
    while(m_pArray[i]<pivot) i++;
    while(m_pArray[j]>pivot) j--;
    if (i<=j){
      T t = m_pArray[i];
      m_pArray[i] = m_pArray[j];
      m_pArray[j] = t;
      i++;
      j--;
    }
  }
  if (l < j) QSort(l, j);
  if (i < r) QSort(i, r);
}

template <class T>
ICVector<T>& ICVector<T>::SetSize(int new_size) {
  Grow(new_size);
  if (new_size == m_nArrayDim) m_nArrayFill = new_size;  
  return *this;
}

template <class T>
ICVector<T>& ICVector<T>::Grow(int new_size){
  if (new_size > m_nArrayDim){
    m_nArrayDim = new_size;
    T* m_pArray_new = new T[m_nArrayDim];
    for (int i=0;i<m_nArrayFill;i++)
      m_pArray_new[i]=m_pArray[i];
    delete[] m_pArray;
    m_pArray = m_pArray_new;	
  }  
  return *this;
}

template <class T>
int ICVector<T>::AddEltSorted(const T& Elt){
  int i=0;
  while (i < m_nArrayFill){
    if (m_pArray[i] >= Elt) break;
    else i++;
  }
  AddElt(Elt, i);
  return i;
}

template <class T> 
int ICVector<T>::AddSortedElt(const T& Value) {
  int pos=FindSortedEltPos(Value,0,m_nArrayFill-1);
  if (pos>=0) AddElt(Value,pos);  
  return pos;
}

template <class T>
int ICVector<T>::FindSortedElt(const T& Value,int min,int max) {
  register int middle;
  if (Value>m_pArray[max]) return -1;
  if (Value<m_pArray[min]) return -1;
  middle=min+(max-min)/2;
  if (min==max-1) return -1;
  if (Value<m_pArray[middle]) return FindSortedElt(Value,min,middle);
  else if (Value>m_pArray[middle]) return FindSortedElt(Value,middle,max);
  else return middle;
}

template <class T>
int ICVector<T>::FindSortedEltPos(const T& Value,int min,int max) {
  register int middle; 
  if (Value>m_pArray[max]) return max+1;
  if (Value<m_pArray[min]) return min;
  middle=min+(max-min)/2;
  if (Value==m_pArray[middle]) return -1;
  if (min==max-1) return max;
  if (Value<m_pArray[middle]) return FindSortedEltPos(Value,min,middle);
  else if (Value>m_pArray[middle]) return FindSortedEltPos(Value,middle,max);
  else return middle;  
}

template <class T>
ICVector<T>& ICVector<T>::AddElt(const T& Elt){	
  if (m_nArrayFill >= m_nArrayDim) {
    m_nArrayDim+=_MIN(MaxFactor,((int)(m_nArrayDim*XFactor) + 1));
    T* m_pArray_new = new T[m_nArrayDim];
    for (int i=0;i<m_nArrayFill;i++)
      m_pArray_new[i]=m_pArray[i];
    delete[] m_pArray;
    m_pArray = m_pArray_new;	
  }
  m_pArray[m_nArrayFill++] = Elt; //((* new T) = Elt);
  return *this;
}

template <class T>
ICVector<T>& ICVector<T>::DeleteEltAt(int nIndex) {  
#ifdef _DEBUG_CPP
  assert(nIndex < m_nArrayFill);
#endif
  if (nIndex >= 0){
    for (int i = nIndex; i < m_nArrayFill - 1; i++)
      m_pArray[i] = m_pArray[i + 1];
    m_nArrayFill--;
  }  
  Resize();
  return (* this);
}

template <class T>
int ICVector<T>::FindElt(const T& Value) {
  for (int i = 0; i < m_nArrayFill; i++)
    if (m_pArray[i]==Value) {
      return i;
    }
  return -1;
}

template <class T>
ICVector<T>& ICVector<T>::AddElt(const T& Elt, int Pos){
#ifdef _DEBUG_CPP
  assert(Pos >= 0);
  assert(Pos <= m_nArrayFill);
#endif  
  if (m_nArrayFill >= m_nArrayDim){
    m_nArrayDim+=_MIN(MaxFactor, ((int)(m_nArrayDim*XFactor) + 1));
    T* m_pArray_new = new T[m_nArrayDim];
    int i=0;
    for (i=0;(i<Pos)&&(i<m_nArrayFill);i++)
      m_pArray_new[i]=m_pArray[i];
    m_pArray_new[i] = Elt;
    for (;i<m_nArrayFill;i++)
      m_pArray_new[i+1]=m_pArray[i];
    delete[] m_pArray;
    m_pArray = m_pArray_new;
    m_nArrayFill++;
  } else {   
    for (int i=m_nArrayFill-1;i>=Pos;i--){
      m_pArray[i+1] = m_pArray[i];
    }   
    m_pArray[Pos] = Elt;
    m_nArrayFill++;
  }
  return *this;
}


/**************
  CVector<T> 
  ************/

template <class T>
void CVector<T>::ICSCopy(int ){ 
  if (m_IContainer->RefCount() > 1)
    m_IContainer = m_IContainer->ICVectorCopy();  
}

template <class T>
CVector<T>::CVector(void){
  m_IContainer = new ICVector<T>(5);
}

template <class T>
CVector<T>::CVector(int iSize){
  m_IContainer = new ICVector<T>(iSize);
}

template <class T>
CVector<T>::CVector(const CVector<T>& rhs){
  m_IContainer = rhs.m_IContainer;
  m_IContainer->Reg();
}

template <class T>
CVector<T>::~CVector(void){
  m_IContainer->UnReg();
}

template <class T>
void CVector<T>::operator=(const CVector<T>& rhs) {
  m_IContainer->UnReg();
  m_IContainer = rhs.m_IContainer;
  m_IContainer->Reg();  
}

template <class T>
void CVector<T>::Clear(void){  
  if (m_IContainer->RefCount() == 1) {
    m_IContainer->m_nArrayFill = 0;
    m_IContainer->Resize();
  } else {
    m_IContainer->UnReg();
    m_IContainer = new ICVector<T>(5);;
  }      
}

template <class T>
CVector<T>& CVector<T>::Grow(int nSize){  
  ICSCopy();
  m_IContainer->Grow(nSize);
  return (* this);
}

template <class T>
CVector<T>& CVector<T>::SetSize(int nSize) {
  ICSCopy();
  m_IContainer->SetSize(nSize);
  return (* this);
}

template <class T> 
int CVector<T>::AddSortedElt(const T& Value)  {
  return m_IContainer->AddSortedElt(Value);
}

template <class T>
CVector<T>& CVector<T>::AddElt(const T& Elt){	
  ICSCopy();
  m_IContainer->AddElt(Elt);
  return (* this);
}

template <class T>
CVector<T>& CVector<T>::DeleteEltAt(int nIndex){
  ICSCopy();
  m_IContainer->DeleteEltAt(nIndex);
  return (* this);
}

template <class T>
int CVector<T>::operator<(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 0;
  if (Count() > Other.Count()) return 0;
  else if (Count() < Other.Count()) return 1;
  else 
    for (int i=0;i<Count();i++) {
      if (GetElt(i) > Other[i]) return 0;
      else if (GetElt(i) < Other[i]) return 1;
    }
  return 0;
}

template <class T>
int CVector<T>::operator>(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 0;
  if (Count() < Other.Count()) return 0;
  else if (Count() > Other.Count()) return 1;
  else 
    for (int i=0;i<Count();i++) {
      if (GetElt(i) < Other[i]) return 0;
      else if (GetElt(i) > Other[i]) return 1;
    }
  return 0;
}

template <class T>
int CVector<T>::operator<=(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 1;
  if (((* this)<Other)||((* this)==Other)) return 1; else return 0;
}

template <class T>
int CVector<T>::operator>=(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 1;
  if (((* this)>Other)||((* this)==Other)) return 1; else return 0;
}

template <class T>
int CVector<T>::operator==(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 1;
  if (Count() != Other.Count()) return 0;
  else for (int i=0;i<Count();i++)
    if (!(GetElt(i) == Other[i])) return 0;
  return 1;
}

template <class T>
int CVector<T>::operator!=(const CVector<T>& Other) const {
  if (m_IContainer == Other.m_IContainer) return 0;
  return (!((* this)==Other));
}

template <class T>
ostream& operator<< (ostream& out, const CVector<T>& rhs) {
  int nMax = rhs.Count();
  for (int i = 0; i < nMax; i++)
    /* signature will match the element << operator */
    out << rhs.GetElt(i) << "\n";
  return out;
}

template <class T> 
ofstream& operator<< (ofstream& fout, const CVector<T>& rhs) {
  int nMax = rhs.Count();
  for (int i = 0; i < nMax; i++)
    fout << rhs.GetElt(i) << "\n";
  return fout;
}

template <class T> 
int CVector<T>::AddEltSorted(const T& Elt){
  ICSCopy();
  return m_IContainer->AddEltSorted(Elt);
}

template <class T> 
int CVector<T>::AddEltSortedUnique(const T& Elt){  
  int i=0;
  while (i < m_IContainer->m_nArrayFill){
    if (m_IContainer->m_pArray[i] == Elt) return -1;
    else if (m_IContainer->m_pArray[i] > Elt) break;
    else i++;
  }
  AddElt(Elt, i);
  return i;
}

template <class T>
CVector<T>& CVector<T>::AddElt(const T& Elt, int Pos){
  ICSCopy();
  m_IContainer->AddElt(Elt, Pos);  
  return *this;
}

#ifdef ANSI
template <class T>
cgiOutStream& operator<< (cgiOutStream& out, const CVector<T>& rhs) {
  int nMax = rhs.Count();
  for (int i = 0; i < nMax; i++)
    /* signature will match the element << operator */
    out << rhs.GetElt(i) << "\n";
  return out;
}
#endif

#endif
