#pragma once
#include "Element.h"

class CElementList
{
public:
	CElementList(void);
	~CElementList(void);
	CElementList & operator = (const CElementList& ELList);

public://nIndex is 0 based
	void ReMoveAll();
	void ReMoveElement(int nIndex);
	void AddElement(PElement pelement);

	int GetElementCount() const;
	PElement GetElement(int nIndex) const;

public:
	int		m_nCount;
	PElement	m_pEndElement;
	PElement	m_pBeginElement;
};
