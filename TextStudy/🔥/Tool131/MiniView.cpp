// MiniView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool131.h"
#include "MiniView.h"
#include "Device.h"
#include "MainFrm.h"


// CMiniView

IMPLEMENT_DYNCREATE(CMiniView, CView)

CMiniView::CMiniView()
{

}

CMiniView::~CMiniView()
{
}

BEGIN_MESSAGE_MAP(CMiniView, CView)
END_MESSAGE_MAP()


// CMiniView 그리기입니다.

void CMiniView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.

	// AfxGetMainWnd : 메인 쓰레드의 핸들을 가져옴
	//CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	
	//AfxGetApp : 메인 쓰레드를 갖고 있는 현재 메인 app정보를 얻어옴
	CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

	//GetParentFrame : 현재 view를 관리하는 상위 메인 프레임 정보를 반환
	//CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(GetParentFrame());

	CTool131View*	pMainView = dynamic_cast<CTool131View*>(pMainFrm->m_MainSplitter.GetPane(0, 1));

	CMyTerrain*		pMyTerrain = pMainView->m_pMyTerrain;
	
	CDevice::Get_Instance()->Render_Begin();

	pMyTerrain->Mini_Render();

	CDevice::Get_Instance()->Render_End(m_hWnd);

}


// CMiniView 진단입니다.

#ifdef _DEBUG
void CMiniView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CMiniView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMiniView 메시지 처리기입니다.
