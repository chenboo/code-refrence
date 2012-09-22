#pragma once

typedef struct Element
{	

	struct Element*		m_pNextElement;
	struct Element*		m_pPrevElement;

	Element()
	{
		m_pPrevElement = NULL;
		m_pNextElement = NULL;
	}
	
	Element(int nxvalue, int nyvalue)
	{
		m_pPrevElement = NULL;
		m_pNextElement = NULL;
	}
	
	Element(const Element& stgph)
	{
		m_pPrevElement = stgph.m_pPrevElement;
		m_pNextElement = stgph.m_pNextElement;
	}	
	
	Element & operator = (const Element& stgph)
	{
		m_pPrevElement = stgph.m_pPrevElement;
		m_pNextElement = stgph.m_pNextElement;
	}
	
	~Element()
	{
		m_pPrevElement = NULL;
		m_pNextElement = NULL;
	}
	
}Element, *PElement;