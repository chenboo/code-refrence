#include "StdAfx.h"
#include <afxtempl.h>
#include "ButtonManager.h"

CButtonManager::CButtonManager(void)
{
	m_nActiveRadbtn = -1;
	m_bHasButton = FALSE;
	m_NorbtnArray.SetSize(0, 1);
	m_RadbtnArray.SetSize(0, 1);
	m_TogbntArray.SetSize(0, 1);
}

CButtonManager::~CButtonManager(void)
{
}

void CButtonManager::CreateButtonManager(int nIconId[],
										 int nBtnID[],
										 int nStyle[],
										 CRect rects[],
										 int nCount,
										 HWND hparent)
{
	m_NorbtnArray.RemoveAll();
	m_RadbtnArray.RemoveAll();
	m_TogbntArray.RemoveAll();

	CDrawButton btn;

	for(int i = 0 ; i < nCount; i++)
	{
		btn.Create(nBtnID[i], hparent, rects[i]);
		btn.SetPNGImage(nIconId[i]);
		btn.SetButtonStyle((BT_STYLE)nStyle[i]);

		if(nStyle[i] == BT_NORMAL)
		{
			m_NorbtnArray.Add(btn);
		}
		else if(nStyle[i] == BT_RADIO)
		{
			m_RadbtnArray.Add(btn);
		}
		else if(nStyle[i] == BT_TOGGLE)
		{
			m_TogbntArray.Add(btn);
		}
	}

	m_bHasButton = TRUE;
}

BT_STYLE CButtonManager::IsPtInButtonsRect(CPoint point)
{
	int nCount = (int)m_NorbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_NorbtnArray[i].IsptInButton(point))
			return BT_NORMAL;
	}

	nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if((m_RadbtnArray[i].IsptInButton(point)))
			return BT_RADIO;
	}

	nCount = (int)m_TogbntArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_TogbntArray[i].IsptInButton(point))
			return BT_TOGGLE;
	}

	return BT_NULL;
}

void CButtonManager::MouseState(UINT nFlags, CPoint point)
{
	if(MU_MOVE == nFlags || MU_MOUSELEAVE == nFlags)
	{
		NormalMouseState(nFlags, point);
		RadioMouseState(nFlags, point);
		ToggleMouseState(nFlags, point);
	}
	else
	{
		BT_STYLE nBtnStyle = IsPtInButtonsRect(point);

		if(BT_NORMAL == nBtnStyle)
		{
			NormalMouseState(nFlags, point);
		}
		else if(BT_RADIO == nBtnStyle)
		{
			RadioMouseState(nFlags, point);
		}
		else if(BT_TOGGLE == nBtnStyle)
		{
			ToggleMouseState(nFlags, point);
		}
		else if(BT_NULL == nBtnStyle)
		{
			if(MU_LBTNMOUSE == nFlags)
			{
				NormalMouseState(nFlags, point);
				RadioMouseState(nFlags, point);
				ToggleMouseState(nFlags, point);
			}
		}
	}
}

void CButtonManager::NormalMouseState(UINT nFlags, CPoint point)
{
	int nCount = (int)m_NorbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_NorbtnArray[i].MouseState(nFlags, point);
	}
}

void CButtonManager::RadioMouseState(UINT nFlags, CPoint point)
{
	int nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_RadbtnArray[i].MouseState(nFlags, point);

		if(m_RadbtnArray[i].GetButtonState() == ST_SELECTED)
		{
			m_nActiveRadbtn = i;
		}
	}
}

void CButtonManager::ToggleMouseState(UINT nFlags, CPoint point)
{
	int nCount = (int)m_TogbntArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_TogbntArray[i].MouseState(nFlags, point);
	}
}

void CButtonManager::DrawButtons(CDC* pDC)
{
	int nCount = (int)m_NorbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_NorbtnArray[i].DrawButton(pDC);
	}

	nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_RadbtnArray[i].DrawButton(pDC);
	}

	nCount = (int)m_TogbntArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		m_TogbntArray[i].DrawButton(pDC);
	}
}

int CButtonManager::GetActiveRadio()
{
	return m_nActiveRadbtn;
}

BOOL CButtonManager::IsHasButton()
{
	return m_bHasButton;
}

UINT CButtonManager::GetActiveRadioID()
{
	if(m_RadbtnArray.GetCount() && m_nActiveRadbtn >= 0)
	{
		return m_RadbtnArray[m_nActiveRadbtn].GetButtonID();
	}
	else
	{
		return -1;
	}
}

void CButtonManager::RestoreActiveRadio()
{
	ASSERT(m_nActiveRadbtn >= 0);
	if(m_nActiveRadbtn >= 0)
	{
		m_RadbtnArray[m_nActiveRadbtn].SetButtonState(ST_SELECTED);
	}
}

void CButtonManager::SetActiveRadio(int nID)
{
	int nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_RadbtnArray[i].GetButtonID() == nID)
		{
			m_nActiveRadbtn = i;
			m_RadbtnArray[i].SetButtonState(ST_SELECTED);
		}
		else
		{
			m_RadbtnArray[i].SetButtonState(ST_NONE);
		}

		m_RadbtnArray[i].SetClickDown(FALSE);
	}
}

void CButtonManager::ResetAllRadioButton()
{
	ASSERT((int)m_RadbtnArray.GetCount());

	if(m_nActiveRadbtn >= 0)
	{
		m_RadbtnArray[m_nActiveRadbtn].SetClickDown(FALSE);
		m_RadbtnArray[m_nActiveRadbtn].SetButtonState(ST_NONE);

		m_nActiveRadbtn = -1;
	}
}

void CButtonManager::SetButtonToggle(int nID, BOOL bToggled)
{
	int nCount = (int)m_TogbntArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_TogbntArray[i].GetButtonID() == nID)
		{
			m_TogbntArray[i].SetButtonToggle(bToggled);
			break;
		}
	}
}

void CButtonManager::EnableNormalButton(int nbtnID, BOOL bEnable)
{
	int nCount = (int)m_NorbtnArray.GetCount();

	for(int i = 0; i < nCount; i++)
	{
		if(m_NorbtnArray[i].GetButtonID() == nbtnID)
		{
			m_NorbtnArray[i].EnableButtun(bEnable);
			return;
		}
	}
}

void CButtonManager::EnableRadioButton(int nbtnID, BOOL bEnable)
{
	int nCount = (int)m_RadbtnArray.GetCount();

	for(int i = 0; i < nCount; i++)
	{
		if(m_RadbtnArray[i].GetButtonID() == nbtnID)
		{
			m_RadbtnArray[i].EnableButtun(bEnable);
			return;
		}
	}
}

void CButtonManager::SetRadioButtonEnhance(int nbtnID, BOOL bEnhance)
{
	int nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_RadbtnArray[i].GetButtonID() == nbtnID)
		{
			m_RadbtnArray[i].SetEnhanceRadio(bEnhance);
		}
	}
}

void CButtonManager::SetRadioButtonMenu(int nbtnID, BOOL bMenu)
{
	int nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_RadbtnArray[i].GetButtonID() == nbtnID)
		{
			m_RadbtnArray[i].SetMenuRadio(bMenu);
		}
	}
}

CRect CButtonManager::GetButtonRect(int nID)
{
	int nCount = (int)m_NorbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_NorbtnArray[i].GetButtonID() == nID)
		{
			return m_NorbtnArray[i].GetButtonRect();
		}
	}

	nCount = (int)m_RadbtnArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_RadbtnArray[i].GetButtonID() == nID)
		{
			return m_RadbtnArray[i].GetButtonRect();
		}
	}

	nCount = (int)m_TogbntArray.GetCount();
	for(int i = 0; i < nCount; i++)
	{
		if(m_TogbntArray[i].GetButtonID() == nID)
		{
			return m_TogbntArray[i].GetButtonRect();
		}
	}

	return CRect(0, 0, 0, 0);
}