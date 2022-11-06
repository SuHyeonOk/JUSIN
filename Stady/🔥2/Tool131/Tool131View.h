
// Tool131View.h : CTool131View Ŭ������ �������̽�
//

#pragma once

#include "Device.h"
#include "MyTerrain.h"
#include "MyForm.h"
#include "MiniView.h"

class CTool131Doc;
class CTool131View : public CScrollView
{
protected: // serialization������ ��������ϴ�.
	CTool131View();
	DECLARE_DYNCREATE(CTool131View)

// Ư���Դϴ�.
public:
	CTool131Doc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CTool131View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

public:
	CDevice*		m_pGraphicDev;
	CMyTerrain*		m_pMyTerrain = nullptr;

	
};

#ifndef _DEBUG  // Tool131View.cpp�� ����� ����
inline CTool131Doc* CTool131View::GetDocument() const
   { return reinterpret_cast<CTool131Doc*>(m_pDocument); }
#endif

