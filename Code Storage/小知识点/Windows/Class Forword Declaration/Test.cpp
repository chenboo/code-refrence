#include "Test.h"
#include "Foo.h"

CTest::CTest()
{
	m_pFoo = new CFoo(50,30);
}

CTest::~CTest()
{
	delete m_pFoo;
}

int CTest::GetSum()
{
	int nx = m_pFoo->GetXvalue();
	int ny = m_pFoo->Getyvalue();

	return nx + ny;
}

int CTest::GetMargin()
{
	int nx = m_pFoo->GetXvalue();
	int ny = m_pFoo->Getyvalue();

	return nx - ny;
}
