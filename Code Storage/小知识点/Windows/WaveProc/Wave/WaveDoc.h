// WaveDoc.h : interface of the CWaveDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEDOC_H__0D3A1F2F_428B_47A6_BE9D_4A7747AB4B97__INCLUDED_)
#define AFX_WAVEDOC_H__0D3A1F2F_428B_47A6_BE9D_4A7747AB4B97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWaveDoc : public CDocument
{
protected: // create from serialization only
	CWaveDoc();
	DECLARE_DYNCREATE(CWaveDoc)

// Attributes
public:
	BOOL fileOpened;
	WORD channels;
	DWORD n;
	float *dm, *dl, *dr;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void stop();
	CString m_path;
     void paly();
	virtual ~CWaveDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWaveDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEDOC_H__0D3A1F2F_428B_47A6_BE9D_4A7747AB4B97__INCLUDED_)
