// WaveView.cpp : implementation of the CWaveView class
//

#include "stdafx.h"
#include "Wave.h"

#include "WaveDoc.h"
#include "WaveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveView

IMPLEMENT_DYNCREATE(CWaveView, CView)

BEGIN_MESSAGE_MAP(CWaveView, CView)
	//{{AFX_MSG_MAP(CWaveView)
	ON_COMMAND(ID_EDIT_PLAY, OnEditPlay)
	ON_COMMAND(ID_EDIT_STOP, OnEditStop)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_PLAY, OnPlay)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveView construction/destruction

CWaveView::CWaveView()
{
	// TODO: add construction code here

	// create color pens
	pRedPen = new CPen(PS_SOLID, 0, RGB(255, 0, 0));
	pGreenPen = new CPen(PS_SOLID, 0, RGB(0, 255, 0));
	pBluePen = new CPen(PS_SOLID, 0, RGB(0, 0, 255));
	pBlackPen = new CPen(PS_SOLID, 0, RGB(0, 0, 0));
}

CWaveView::~CWaveView()
{
}

BOOL CWaveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWaveView drawing

void CWaveView::OnDraw(CDC* pDC)
{  
	CWaveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
     
	if (!pDoc->fileOpened) return;


   
	




	//HCURSOR hOldCursor=SetCursor(LoadCursor(NULL, IDC_WAIT));
   
	RECT crect;
	GetClientRect(&crect);
	int w=crect.right, h=crect.bottom, h0 = h / 2;
	long n = pDoc->n;
	float dx = (float)w / n, dy = (float)h / 2;
	int i, x, y;
	// MSG message;

	TRACE(_T("n = %d\n"), n);
	
	if(pDoc->channels == 1) { // draw channel of mono samples
		// draw zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);
		// draw sample data
		pDC->SelectObject(pGreenPen);
		for(i = 0; i< n; i++) {
			x = (int)(i * dx + 0.5); 
			y = h0 - (int)(pDoc->dm[i] * dy + 0.5);
			if(i == 0) pDC->MoveTo(x, y); else pDC->LineTo(x, y);
			//::PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
		}
		// draw zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);
	}
	else { // draw channels of stereo samples
		dy /= 2; h0 /= 2;

		// draw samples of left channel
		// draw upper zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);
		// draw sample data
		pDC->SelectObject(pGreenPen);
		for(i = 0; i< n; i++) {
			x = (int)(i * dx + 0.5); 
			y = h0 - (int)(pDoc->dl[i] * dy + 0.5);
			if(i == 0) pDC->MoveTo(x, y); else pDC->LineTo(x, y);
			//::PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
			TRACE(_T("play i = %d\n"), i);
		}
		// draw upper zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);

		// draw seperator line
		pDC->SelectObject(pBlackPen);
		pDC->MoveTo(0, h / 2); pDC->LineTo(w, h / 2);
		
		// draw samples of right channel
		h0 += h / 2;
		// draw lower zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);
		// draw sample data
		pDC->SelectObject(pRedPen);
		for(i = 0; i< n; i++) {
			x = (int)(i * dx + 0.5); 
			y = h0 - (int)(pDoc->dr[i] * dy + 0.5);
			if(i == 0) pDC->MoveTo(x, y); else pDC->LineTo(x, y);
			//::PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
		}
		// draw lower zero line
		pDC->SelectObject(pBluePen);
		pDC->MoveTo(0, h0); pDC->LineTo(w, h0);
	}
	//SetCursor(hOldCursor);
	
}

/////////////////////////////////////////////////////////////////////////////
// CWaveView printing

BOOL CWaveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWaveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWaveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CWaveView diagnostics

#ifdef _DEBUG
void CWaveView::AssertValid() const
{
	CView::AssertValid();
}

void CWaveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWaveDoc* CWaveView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWaveDoc)));
	return (CWaveDoc*)m_pDocument;
	
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWaveView message handlers

void CWaveView::OnEditPlay() 
{
	// TODO: Add your command handler code here
	CWaveDoc *temp=GetDocument();
	temp->paly();//
	
}

void CWaveView::OnEditStop() 
{
	// TODO: Add your command handler code here
		CWaveDoc *temp=GetDocument();
	temp->stop();//
	
}

void CWaveView::OnStop() 
{
	// TODO: Add your command handler code here
	OnEditStop();
}

void CWaveView::OnPlay() 
{
	// TODO: Add your command handler code here
	CDC *pDC;
	pDC=GetDC();
	OnDraw(pDC);
	OnEditPlay();
}
