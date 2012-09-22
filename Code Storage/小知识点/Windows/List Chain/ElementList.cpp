#include "stdafx.h"
#include "ElementList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CElementList::CElementList(void)
{
	m_nCount = 0;
	m_pEndElement = NULL;
	m_pBeginElement = NULL;
}

CElementList::~CElementList(void)
{
	ReMoveAll();
}

CElementList & CElementList::operator = (const CElementList& ELList)
{
	ReMoveAll();

	PElement pEle = ELList.m_pBeginElement;

	for(int i = 0; i < ELList.m_nCount; i++)
	{
		ASSERT(pEle);
		AddElement(pEle);
		pEle = pEle->m_pNextElement;
	}

	return *this;
}

void CElementList::ReMoveAll()
{
	PElement pEle = m_pBeginElement;
	for(int i = 0; i < m_nCount; i++)
	{
		ASSERT(pEle);
		m_pBeginElement = m_pBeginElement->m_pNextElement;
		delete pEle;
		pEle = m_pBeginElement;
	}

	m_nCount = 0;
	m_pBeginElement = m_pEndElement = NULL;
}

PElement CElementList::GetElement(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	PElement pEle = NULL;

	for(int i = 0; i <= nIndex;i++)
	{
		if(i == 0)
		{
			pEle = m_pBeginElement;
		}
		else
		{
			pEle = pEle->m_pNextElement;
		}
	}

	ASSERT(pEle);
	return pEle;
}

int CElementList::GetElementCount() const
{
	return m_nCount;
}

void CElementList::ReMoveElement(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	PElement pEle1 = NULL;
	
	for(int i = 0; i <= nIndex; i++)
	{
		if(i == 0)
		{
			pEle1 = m_pBeginElement;
		}
		else
		{
			pEle1 = pEle1->m_pNextElement;
		}
	}

	ASSERT(pEle1);
	PElement pEle2 = pEle1->m_pNextElement;
	PElement pEle3 = pEle1->m_pPrevElement;

	if(pEle2)
	{
		pEle2->m_pPrevElement = pEle3;
	}
	else
	{
		m_pEndElement = pEle3;
	}

	if(pEle3)
	{
		pEle3->m_pNextElement = pEle2;
	}
	else
	{
		m_pBeginElement = pEle2;
	}

	delete pEle1;
	pEle1 = NULL;
	m_nCount--;
}

void CElementList::AddElement(PElement pelement)
{
	ASSERT(pelement);

	if(!pelement)
	{
		return;
	}

	if(m_nCount == 0)
	{
		m_pBeginElement = m_pEndElement = pelement;
		m_pEndElement->m_pNextElement = NULL;
		m_pEndElement->m_pPrevElement = NULL;
	}
	else
	{
		m_pEndElement->m_pNextElement = pelement;
		pelement->m_pPrevElement = m_pEndElement;
		pelement->m_pNextElement = NULL;
		m_pEndElement = pelement;
	}

	m_nCount++;
}
