#ifndef __NONINTRUSIVELIST_H3D
#define __NONINTRUSIVELIST_H3D
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "basetypes.h"

//----------------------------------------------------------------------
#define ARRAY_CHUNK_SIZE 100

//----------------------------------------------------------------------
template<class TYPE>
class SFArrayBase
{
protected:
	SFInt32       m_nSize;
	SFInt32       m_nValues;
	TYPE         *m_Values;

public:
	              SFArrayBase   (void);
	              SFArrayBase   (const SFArrayBase& copy);
	             ~SFArrayBase   (void);

	SFArrayBase&  operator=	    (const SFArrayBase& copy);

	      TYPE&   operator[]    (SFInt32 index);
	const TYPE&   operator[]    (SFInt32 index) const;

		  // syntactic sugar
	      void    addValue      (TYPE val)    { operator[](m_nValues) = val; }
	      SFInt32 getCount      (void) const  { return m_nValues; }
	      SFInt32 getSize       (void) const  { return m_nSize;   }

		  void    Sort          (SORTINGFUNC func) { qsort(&m_Values[0], m_nValues, sizeof(TYPE), func); }
	TYPE    Find          (TYPE key, SEARCHFUNC func) { return (TYPE)bsearch(key, &m_Values[0], m_nValues, sizeof(TYPE), func); }
	
	      void    Clear         (void);

private:
		  void    CheckSize     (SFInt32 sizeNeeded);
	      void    Copy          (const SFArrayBase& copy);
	      void    Init          (SFInt32 size, SFInt32 count, TYPE *values);
};

//----------------------------------------------------------------------
template<class TYPE>
inline SFArrayBase<TYPE>::SFArrayBase(void)
{
	Init(0, 0, NULL);
}

//----------------------------------------------------------------------
template<class TYPE>
inline SFArrayBase<TYPE>::SFArrayBase(const SFArrayBase<TYPE>& copy)
{
	Copy(copy);
}

//----------------------------------------------------------------------
template<class TYPE>
inline SFArrayBase<TYPE>::~SFArrayBase(void)
{
	Clear();
}

//----------------------------------------------------------------------
template<class TYPE>
inline SFArrayBase<TYPE>& SFArrayBase<TYPE>::operator=(const SFArrayBase<TYPE>& copy)
{
	Clear();
	Copy(copy);
	return *this;
}

//----------------------------------------------------------------------
template<class TYPE>
inline void SFArrayBase<TYPE>::Init(SFInt32 size, SFInt32 count, TYPE *values)
{
	m_nSize   = size;
	m_nValues = count;
	m_Values  = values;
}

//----------------------------------------------------------------------
template<class TYPE>
inline void SFArrayBase<TYPE>::Clear(void)
{
	if (m_Values)
		delete [] m_Values;
	Init(0, 0, NULL);
}

//----------------------------------------------------------------------
template<class TYPE>
inline void SFArrayBase<TYPE>::Copy(const SFArrayBase<TYPE>& copy)
{
	CheckSize(copy.getSize());
	for (int i=0;i<copy.getCount();i++)
		m_Values[i] = copy.m_Values[i];
	m_nValues = copy.getCount();
}

//----------------------------------------------------------------------
template<class TYPE>
inline void SFArrayBase<TYPE>::CheckSize(SFInt32 sizeNeeded)
{
	if (sizeNeeded < m_nSize)
		return;

	// The user is requesting access to an index that
	// is past range.  We need to grow the array.
	SFInt32 newSize = MAX(m_nSize + ARRAY_CHUNK_SIZE, sizeNeeded);
	TYPE *newArray  = new TYPE[newSize];
	if (m_nValues)
	{
		ASSERT(m_Values);
		// if there are any values in the source copy them over
		for (int i=0;i<m_nValues;i++)
			newArray[i] = m_Values[i];
		// then clear out the old array
		if (m_Values)
			delete [] m_Values;
		m_Values = NULL;
	}
	Init(newSize, m_nValues, newArray);
}

//----------------------------------------------------------------------
template<class TYPE>
inline TYPE& SFArrayBase<TYPE>::operator[](SFInt32 index)
{
	// This is the non-const version which means we may
	// have to grow the array
	CheckSize(index);
	if (index >= m_nValues)
		m_nValues = index+1;
	ASSERT(m_Values && index >= 0 && index <= m_nSize && index <= m_nValues);
	return m_Values[index];
}

//----------------------------------------------------------------------
template<class TYPE>
inline const TYPE& SFArrayBase<TYPE>::operator[](SFInt32 index) const
{
	// This is the const version which means its a get which
	// means we should not be expecting the thing to grow.
	// Does not appear to protect against accessing outside range though
	ASSERT(index>=0 && index<=m_nValues);
	return m_Values[index];
}

#if 0
/*

//----------------------------------------------------------------------
template<class TYPE>
class SFArray : public SFArrayBase < TYPE >
{
public:
	friend SFBool operator==(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2);
	friend SFBool operator!=(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2);
	SFArray<TYPE>& operator=(const SFArray<TYPE>& v1);
	friend SFArray<TYPE> operator+(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2);
	friend SFArray<TYPE> operator-(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2);
	friend SFArray<TYPE> operator*(const SFArray<TYPE>& v,  SFFloat f);
	friend SFArray<TYPE> operator/(const SFArray<TYPE>& v,  SFFloat f);
};

//---------------------------------------------------------------------
template<class TYPE>
inline SFArray<TYPE> operator+(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2)
{
	SFArray<TYPE> array;
	ASSERT(v1.GetCount() == v2.GetCount());
	for (int i=0;i<v1.GetCount();i++)
	{
		array.AddValue(v1[i] + v2[i]);
	}
	return array;
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFArray<TYPE> operator-(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2)
{
	SFArray<TYPE> array;
	ASSERT(v1.GetCount() == v2.GetCount());
	for (int i=0;i<v1.GetCount();i++)
	{
		array.AddValue(v1[i] - v2[i]);
	}
	return array;
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFArray<TYPE> operator*(const SFArray<TYPE>& v,  SFFloat f)
{
	SFArray<TYPE> array;
	for (int i=0;i<v.GetCount();i++)
	{
		array.AddValue(v[i] * f);
	}
	return array;
}

template<class TYPE>
inline SFArray<TYPE> operator/(const SFArray<TYPE>& v,  SFFloat f)
{
	SFArray<TYPE> array;
	for (int i=0;i<v.GetCount();i++)
	{
		array.AddValue(v[i] / f);
	}
	return array;
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFBool operator==(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2)
{
	if (v1.GetCount() != v2.GetCount())
		return FALSE;

	for (int i=0;i<v1.GetCount();i++)
	{
		if (v1[i] != v2[i])
			return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFBool operator!=(const SFArray<TYPE>& v1, const SFArray<TYPE>& v2)
{
	return (!(operator==(v1, v2)));
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFArray<TYPE>& SFArray<TYPE>::operator=(const SFArray<TYPE>& v1)
{
	SFArrayBase<TYPE>::operator=(v1);
	return *this;
}

*/

#endif

struct xLISTPOS__ { int unused; };
typedef xLISTPOS__* LISTPOS;

//----------------------------------------------------------------------
template<class TYPE>
class SFListNode
{
private:
	// Don't allow default construction, copy construction or assignment
	SFListNode(void) {}
	SFListNode(const SFListNode& node) {}
	SFListNode& operator=(const SFListNode& node) { return *this; }

public:
	SFListNode *m_Next;
	SFListNode *m_Prev;
	TYPE        m_Data;

	            SFListNode(TYPE d)     { m_Next = NULL; m_Prev = NULL;  m_Data = d; }
	           ~SFListNode(void)       { m_Next = NULL; m_Prev = NULL;  }
	SFListNode *Next      (void) const { return m_Next; }
	SFListNode *Prev      (void) const { return m_Prev; }
};

//----------------------------------------------------------------------
template<class TYPE>
class SFList
{
protected:
	long              m_Count;
	SFListNode<TYPE> *m_Head;
	SFListNode<TYPE> *m_Tail;

public:
	         SFList          (void);
	         SFList          (const SFList& l);
	        ~SFList          (void);

	SFList&  operator=       (const SFList& l);

	long     GetCount        (void) const { return m_Count;                }
	TYPE     GetHead         (void) const { return (TYPE)(m_Head->m_Data); }
	TYPE     GetTail         (void) const { return (TYPE)(m_Tail->m_Data); }

	LISTPOS  GetHeadPosition (void) const { return (LISTPOS)m_Head; }
	LISTPOS  GetTailPosition (void) const { return (LISTPOS)m_Tail; }

	void     setHead         (SFListNode<TYPE> *newHead) { m_Head = newHead; }
	void     setTail         (SFListNode<TYPE> *newTail) { m_Tail = newTail; }

	void     AddToList       (TYPE item)  { AddTail(item);           }
	SFBool   IsEmpty         (void) const { return (m_Head == NULL); }

	TYPE     GetNext         (LISTPOS& rPosition) const;
	TYPE     GetPrev         (LISTPOS& rPosition) const;
	LISTPOS  Find            (TYPE item) const;
	TYPE     FindAt          (TYPE item) const;
	TYPE     FindAt          (LISTPOS pos) const;

	void     AddHead         (TYPE item);
	void     AddTail         (TYPE item);
	void     AddToList       (const SFList& l);
	SFBool   AddSorted       (TYPE item, SORTINGFUNC sortFunc, DUPLICATEFUNC dupFunc=NULL);

	void     InsertBefore    (LISTPOS pos, TYPE item);
	void     InsertAfter     (LISTPOS pos, TYPE item);

	TYPE     RemoveAt        (LISTPOS pos);
	TYPE     RemoveHead      (void);
	TYPE     RemoveTail      (void);
	void     RemoveAll       (void);
};

//---------------------------------------------------------------------
template<class TYPE>
inline SFList<TYPE>::SFList(void)
{
	m_Head  = NULL;
	m_Tail  = NULL;
	m_Count = 0; 
}

//---------------------------------------------------------------------
template<class TYPE>
SFList<TYPE>::SFList(const SFList<TYPE>& l)
{
	m_Head  = NULL;
	m_Tail  = NULL;
	m_Count = 0; 

	LISTPOS pos = l.GetHeadPosition();
	while (pos)
	{
		TYPE ob = l.GetNext(pos);
		AddTail(ob);
	}
}

//---------------------------------------------------------------------
template<class TYPE>
inline SFList<TYPE>::~SFList(void)
{
	RemoveAll();
}

//---------------------------------------------------------------------
template<class TYPE>
SFList<TYPE>& SFList<TYPE>::operator=(const SFList<TYPE>& l)
{
	RemoveAll();

	LISTPOS pos = l.GetHeadPosition();
	while (pos)
	{
		TYPE ob = l.GetNext(pos);
		AddTail(ob);
	}
	return *this;
}

//---------------------------------------------------------------------
template<class TYPE>
TYPE SFList<TYPE>::FindAt(TYPE probe) const
{
	LISTPOS pos = GetHeadPosition();
	while (pos)
	{
		TYPE ob = GetNext(pos);
		if (ob == probe)
			return ob;
	}
	return NULL;
}

//---------------------------------------------------------------------
template<class TYPE>
TYPE SFList<TYPE>::FindAt(LISTPOS probe) const
{
	LISTPOS pos = GetHeadPosition();
	while (pos)
	{
		LISTPOS prev = pos;
		TYPE ob = GetNext(pos);
		if (prev == probe)
			return ob;
	}
	return NULL;
}

//---------------------------------------------------------------------
template<class TYPE>
LISTPOS SFList<TYPE>::Find(TYPE probe) const
{
	LISTPOS pos = GetHeadPosition();
	while (pos)
	{
		LISTPOS last = pos;
		TYPE ob = GetNext(pos);
		if (ob == probe)
			return last;
	}
	return NULL;
}

//---------------------------------------------------------------------
template<class TYPE>
inline void SFList<TYPE>::AddHead(TYPE data)
{
	SFListNode<TYPE> *node = new SFListNode<TYPE>(data);

	ASSERT(node);
	ASSERT(!m_Head || m_Head->m_Prev == NULL);
	ASSERT(!m_Tail || m_Tail->m_Next == NULL);

	node->m_Next = m_Head;
	node->m_Prev = NULL;

	if (!m_Head)
	{
		ASSERT(!m_Tail);
		m_Head = m_Tail = node;
	} else
	{
		ASSERT(m_Tail);
		m_Head->m_Prev = node;
		m_Head = node;
	}

	m_Count++;
}

//---------------------------------------------------------------------
template<class TYPE>
inline void SFList<TYPE>::AddTail(TYPE data)
{
	SFListNode<TYPE> *node = new SFListNode<TYPE>(data);

	ASSERT(node);
	ASSERT(!m_Head || m_Head->m_Prev == NULL);
	ASSERT(!m_Tail || m_Tail->m_Next == NULL);

	node->m_Next = NULL;
	node->m_Prev = m_Tail;

	if (!m_Head)
	{
		ASSERT(!m_Tail);
		m_Head = m_Tail = node;
	} else
	{
		ASSERT(m_Tail);
		m_Tail->m_Next = node;
		m_Tail = node;
	}

	m_Count++;
}

//---------------------------------------------------------------------
template<class TYPE>
inline void SFList<TYPE>::InsertBefore(LISTPOS pos, TYPE data)
{
	SFListNode<TYPE> *node   = new SFListNode<TYPE>(data);
	SFListNode<TYPE> *before = (SFListNode<TYPE> *)pos;

	ASSERT(node && before);

	node->m_Prev = before->m_Prev;
	node->m_Next = before;

	if (before->m_Prev)
		before->m_Prev->m_Next = node;
	before->m_Prev = node;

	ASSERT(m_Head && m_Tail); // We would have used AddTail otherwise
	if (before == m_Head)
		m_Head = node;

	m_Count++;
}

//---------------------------------------------------------------------
template<class TYPE>
inline void SFList<TYPE>::InsertAfter(LISTPOS pos, TYPE data)
{
	SFListNode<TYPE> *node  = new SFListNode<TYPE>(data);
	SFListNode<TYPE> *after = (SFListNode<TYPE> *)pos;

	ASSERT(node && after);

	node->m_Prev = after;
	node->m_Next = after->m_Next;

	if (after->m_Next)
		after->m_Next->m_Prev = node;
	after->m_Next = node;

	ASSERT(m_Head && m_Tail); // We would have used AddTail otherwise
	if (after == m_Tail)
		m_Tail = node;

	m_Count++;
}

//---------------------------------------------------------------------
template<class TYPE> 
void SFList<TYPE>::AddToList(const SFList<TYPE>& l)
{
	LISTPOS pos = l.GetHeadPosition();
	while (pos)
	{
		TYPE ob = l.GetNext(pos);
		AddToList(ob);
	}
}

//---------------------------------------------------------------------
template<class TYPE>
inline TYPE SFList<TYPE>::GetNext(LISTPOS& pos) const
{
	SFListNode<TYPE> *node = (SFListNode<TYPE> *)pos;
	ASSERT(node);
	pos = (LISTPOS)((node->m_Next!=m_Head) ? node->m_Next : NULL);

	return (TYPE)(node->m_Data);
}

//---------------------------------------------------------------------
template<class TYPE>
inline TYPE SFList<TYPE>::GetPrev(LISTPOS& pos) const
{
	SFListNode<TYPE> *node = (SFListNode<TYPE> *)pos;
	ASSERT(node);
	pos = (LISTPOS)((node->m_Prev!=m_Tail) ? node->m_Prev : NULL);

	return (TYPE)(node->m_Data);
}

//---------------------------------------------------------------------
template<class TYPE>
inline void SFList<TYPE>::RemoveAll(void)
{ 
	SFListNode<TYPE> *node = m_Head;
	while (node)
	{
		SFListNode<TYPE> *n = ((node->m_Next!=m_Head) ? node->m_Next : NULL);
		if (node == m_Head)
			m_Head = NULL;
		delete node;
		node = n;
	}

	m_Head      = NULL;
	m_Tail      = NULL;
	m_Count     = 0; 
}

//---------------------------------------------------------------------
template<class TYPE>
inline TYPE SFList<TYPE>::RemoveAt(LISTPOS pos)
{
	SFListNode<TYPE> *node = (SFListNode<TYPE> *)pos;

	ASSERT(node);
	ASSERT(!m_Head || m_Head->m_Prev == NULL);
	ASSERT(!m_Tail || m_Tail->m_Next == NULL);

	TYPE data = (TYPE)(node->m_Data);

	if (!m_Head)
	{
		ASSERT(!m_Tail);
		delete node;
		return data;
	}
	ASSERT(m_Tail);

	if (m_Head == node)
		m_Head = m_Head->m_Next;

	if (m_Tail == node)
		m_Tail = m_Tail->m_Prev;

	if (node->m_Prev)
		node->m_Prev->m_Next = node->m_Next;

	if (node->m_Next)
		node->m_Next->m_Prev = node->m_Prev;

	m_Count--;

	delete node;
	return data;
}

//---------------------------------------------------------------------
// stack use
template<class TYPE>
inline TYPE SFList<TYPE>::RemoveHead(void)
{
	return RemoveAt((LISTPOS)m_Head);
}

//---------------------------------------------------------------------
// queue use
template<class TYPE>
inline TYPE SFList<TYPE>::RemoveTail(void)
{
	return RemoveAt((LISTPOS)m_Tail);
}
#include "exportcontext.h"
//-----------------------------------------------------------------------------
// return true of added, false otherwise so caller can free allocated memory if any
template<class TYPE>
inline SFBool SFList<TYPE>::AddSorted(TYPE item, SORTINGFUNC sortFunc, DUPLICATEFUNC dupFunc)
{
	if (!item)
		return FALSE;

	if (sortFunc)
	{
		// Sort it in (if told to)...
		LISTPOS ePos = GetHeadPosition();
		while (ePos)
		{
			LISTPOS lastPos = ePos;
			TYPE test = GetNext(ePos);

			SFBool isDup = dupFunc && (dupFunc)(item, test);
			if (isDup)
			{
				// caller must free this memory or drop it
				return FALSE;
			}

			if ((sortFunc)(item, test) < 0)
			{
				InsertBefore(lastPos, item);
				return TRUE;
			}
		}
	}

	// ...else just add it to the end
	AddToList(item);
	return TRUE;
}

//----------------------------------------------------------------------
typedef SFArrayBase<SFString> SFStringArray;

//----------------------------------------------------------------------
typedef SFList<SFString> SFStringList;

//---------------------------------------------------------------------------------
template<class TYPE>
class SFStack : public SFList<TYPE>
{
public:
    SFStack (void)
    {
    }
    ~SFStack (void)
    {
        while (Peek())
            delete Pop();
    }
    
	void Push (TYPE val) {        SFList<TYPE>::AddHead(val); }
	TYPE Peek (void)     { return SFList<TYPE>::IsEmpty() ? NULL : SFList<TYPE>::GetHead(); }
	TYPE Pop  (void)     { return SFList<TYPE>::RemoveHead(); }
};

#endif
