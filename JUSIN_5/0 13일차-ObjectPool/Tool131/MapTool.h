#pragma once
#include "afxwin.h"

#include "Include.h"
#include <atlimage.h>

// CMapTool ��ȭ �����Դϴ�.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListBox();
	afx_msg void OnDropFiles(HDROP hDropInfo);


	void		HorizontalScroll(void);

public:
	// control
	CListBox m_ListBox;
	CStatic m_Picture;

	// value
	// CImage : jpeg, gif, bmp, png ���� ���� ����

	map<CString, CImage*>		m_MapPngImg;
	int							m_iDrawID = 0;

	afx_msg void OnSaveData();
};
