
// Tool131View.h : CTool131View 클래스의 인터페이스
//

#pragma once

#include "Device.h"
#include "MyTerrain.h"
#include "MyForm.h"
#include "MiniView.h"

class CTool131Doc;
class CTool131View : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CTool131View();
	DECLARE_DYNCREATE(CTool131View)

// 특성입니다.
public:
	CTool131Doc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CTool131View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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

#ifndef _DEBUG  // Tool131View.cpp의 디버그 버전
inline CTool131Doc* CTool131View::GetDocument() const
   { return reinterpret_cast<CTool131Doc*>(m_pDocument); }
#endif

