// MiniView.cpp : ���� �����Դϴ�.
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


// CMiniView �׸����Դϴ�.

void CMiniView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.

	// AfxGetMainWnd : ���� �������� �ڵ��� ������
	//CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	
	//AfxGetApp : ���� �����带 ���� �ִ� ���� ���� app������ ����
	CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());

	//GetParentFrame : ���� view�� �����ϴ� ���� ���� ������ ������ ��ȯ
	//CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(GetParentFrame());

	CTool131View*	pMainView = dynamic_cast<CTool131View*>(pMainFrm->m_MainSplitter.GetPane(0, 1));

	CMyTerrain*		pMyTerrain = pMainView->m_pMyTerrain;
	
	CDevice::Get_Instance()->Render_Begin();

	pMyTerrain->Mini_Render();

	CDevice::Get_Instance()->Render_End(m_hWnd);

}


// CMiniView �����Դϴ�.

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


// CMiniView �޽��� ó�����Դϴ�.
