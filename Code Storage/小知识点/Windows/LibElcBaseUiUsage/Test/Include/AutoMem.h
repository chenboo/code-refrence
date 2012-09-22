#pragma once

template <typename POINTER_TYPE>
class CAutoMemPtr
{
public:
	CAutoMemPtr(int cbMem) {
		m_p = (POINTER_TYPE)malloc(cbMem);
	}

	virtual ~CAutoMemPtr() {
		if (NULL != m_p) {
			free(m_p);
		}
	}

	operator POINTER_TYPE() {
		return m_p;
	}

protected:
	POINTER_TYPE	m_p; 
};