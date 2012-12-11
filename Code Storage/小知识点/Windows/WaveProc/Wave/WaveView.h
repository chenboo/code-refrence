// WaveView.h : interface of the CWaveView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEVIEW_H__6D82679C_7A3A_4A81_B782_B732B7545517__INCLUDED_)
#define AFX_WAVEVIEW_H__6D82679C_7A3A_4A81_B782_B732B7545517__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWaveView : public CView
{
protected: // create from serialization only
	CWaveView();
	DECLARE_DYNCREATE(CWaveView)

// Attributes
public:
	CWaveDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWaveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CPen *pRedPen, *pGreenPen, *pBluePen, *pBlackPen;
	
// Generated message map functions
protected:
	//{{AFX_MSG(CWaveView)
	afx_msg void OnEditPlay();
	afx_msg void OnEditStop();
	afx_msg void OnStop();
	afx_msg void OnPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WaveView.cpp
inline CWaveDoc* CWaveView::GetDocument()
   { return (CWaveDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEVIEW_H__6D82679C_7A3A_4A81_B782_B732B7545517__INCLUDED_)
