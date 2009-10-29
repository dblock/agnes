#ifndef CVECTOR_H
#define CVECTOR_H

#include <iostream.h>
#include <fstream.h>
#include <assert.h>

template <class T> 
class CVector;

template <class T>
class ICVector {
private:
  int m_nArrayDim;  
  int m_nArrayFill;
  T * m_pArray;  
  mutable int m_Refs;
  void AllocSize(int);
  void Resize(void);
 public:
  /*
    constructors and registration pointers
    */
  ICVector<T>(void);
  ICVector<T>(int); 
  ~ICVector(void);
  /*
    reference ops
    */
  inline int RefCount(void) const { return m_Refs; }
  inline void Reg(void) const { m_Refs++; }
  inline void UnReg(void) const { m_Refs--;  if (!m_Refs) delete this; }
  T * DataPointer(void) const { return m_pArray; }
  ICVector<T> * ICVectorCopy(void);   
  ICVector<T>& Grow(int);
  ICVector<T>& SetSize(int);  
  /*
    variant ops
    */
  inline void QSort(void) { if (m_nArrayFill) QSort(0, m_nArrayFill - 1); }
  void QSortUnique(void);
  void QSort(int l, int r);
  ICVector<T>& AddElt(const T&);
  ICVector<T>& AddElt(const T&, int);
  int AddEltSorted(const T&);
  inline ICVector<T>& DeleteElt(int nIndex) { return DeleteEltAt(nIndex); }
  ICVector<T>& DeleteEltAt(int);
  int FindElt(const T&);
  int FindSortedElt(const T& Value,int min,int max);
  int FindSortedEltPos(const T& Value,int min,int max);
  int AddSortedElt(const T& Value);
  friend class CVector<T>;
};

template <class T>
class CVector {
 private:
  ICVector<T> * m_IContainer;
  void ICSCopy(int Size = 5);
 public:
  /*
    constructors, constructor ops
  */
  CVector<T>(void);
  CVector<T>(int);
  CVector<T>(const CVector<T>&);
  inline T * DataPointer(void) const { return m_IContainer->DataPointer(); }
  ~CVector(void);
  void operator=(const CVector<T>&);
  /*
    constant retrievals
  */
  inline T GetElt(const int nIndex) const { return m_IContainer->m_pArray[nIndex]; }
  inline const T& GetRef(const int nIndex) const { return m_IContainer->m_pArray[nIndex]; }
  inline void SetElt(const int nIndex, const T& Elt) { ICSCopy(); m_IContainer->m_pArray[nIndex] = Elt; }
  // inline const T& GetElt(const int nIndex) { ICSCopy(); return m_IContainer->m_pArray[nIndex]; } 
  inline int FindElt(const T& Value) const { return m_IContainer->FindElt(Value); }
  inline int FindSortedElt(const T& Value) const { return m_IContainer->FindSortedElt(Value,0,m_IContainer->m_nArrayFill-1); }
  inline int Contains(const T& Element) const { return FindElt(Element) + 1; }
  inline int Count(void) const { return m_IContainer->m_nArrayFill; }
  inline T operator[](const int nIndex) const { return m_IContainer->m_pArray[nIndex]; }
  inline T& operator[](const int nIndex) {  ICSCopy(); return m_IContainer->m_pArray[nIndex]; }
  int operator==(const CVector<T>&) const;
  int operator>(const CVector<T>&) const;
  int operator<(const CVector<T>&) const;
  int operator>=(const CVector<T>&) const;
  int operator<=(const CVector<T>&) const;
  int operator!=(const CVector<T>&) const;
  /*
    size and element adjustement
  */
  CVector<T>& Grow(int);
  CVector<T>& SetSize(int);
  
  CVector<T>& AddElt(const T&, int);
  CVector<T>& AddElt(const T&);
  int AddSortedElt(const T& Value);
  int AddEltSorted(const T&);
  int AddEltSortedUnique(const T&);  
  inline CVector<T>& operator+=(const T& Element) { return AddElt(Element); }
  inline void operator-=(const T& Element) { DeleteElt(FindElt(Element)); }
  void operator-=(const int nIndex) { DeleteElt(nIndex); }
  inline CVector<T>& DeleteElt(int nIndex) { return DeleteEltAt(nIndex); }
  CVector<T>& DeleteEltAt(int nIndex);
  
  void Clear(void);  
  inline void Flush(CVector<T>& Vector) { Vector = (* this); }
  
  inline void QSort(void) { ICSCopy(); m_IContainer->QSort(); }
  inline void QSortUnique(void) { ICSCopy(); m_IContainer->QSortUnique(); }
};

#endif
