#pragma once

#include "UnitTool.h"
#include "MapTool.h"
#include "MySheet.h"
#include "PathFind.h"

// CMyForm �� ���Դϴ�.

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUnitTool();

private:
	CFont			m_Font;

public:
	CUnitTool		m_UnitTool;
	CMapTool		m_MapTool;
	CMySheet		m_Popup;
	CPathFind		m_PathFind;

public:
	virtual void OnInitialUpdate();
	afx_msg void OnMapTool();
	afx_msg void OnPopup();
	afx_msg void OnPathFind();
};


