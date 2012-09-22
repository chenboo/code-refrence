#include "Foo.h"

CFoo::CFoo(int nx, int ny)
{
	m_nx = nx;
	m_ny = ny;
}

int CFoo::GetXvalue()
{
	return m_nx;
}
	
int CFoo::Getyvalue()
{
	return m_ny;
}

void CFoo::SetXvalue(int nx)
{
	m_nx = nx;
}
	
void CFoo::SetYvalue(int ny)
{
	m_ny = ny;
}