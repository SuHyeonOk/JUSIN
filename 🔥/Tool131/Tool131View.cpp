
// Tool131View.cpp : CTool131View Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool131.h"
#endif

#include "Tool131Doc.h"
#include "Tool131View.h"

#include "TextureMgr.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND		g_hWnd;

// CTool131View

IMPLEMENT_DYNCREATE(CTool131View, CScrollView)

BEGIN_MESSAGE_MAP(CTool131View, CScrollView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CTool131View ����/�Ҹ�

CTool131View::CTool131View()
	: m_pGraphicDev(CDevice::Get_Instance())
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CTool131View::~CTool131View()
{
	Safe_Delete(m_pMyTerrain);

	CTextureMgr::Get_Instance()->Destroy_Instance();
	m_pGraphicDev->Destroy_Instance();
}

void CTool131View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	//	SetScrollSizes : ��ũ�� ���� ����� �����ϴ� �Լ�
	// MM_TEXT : �ȼ� ������ ũ�⸦ �����ϰڴٴ� �ɼ�

	SetScrollSizes(MM_TEXT, CSize(TILEX * TILECX, TILEY * TILECY / 2));
	
	// AfxGetMainWnd: ������ ���� �����츦 ��ȯ�ϴ� ���� �Լ�
	CMainFrame*		pMainFrm = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWnd{};

	// GetWindowRect : ���� �������� RECT ������ ������ �Լ�
	pMainFrm->GetWindowRect(&rcWnd);

	// SetRect : ������ �Ű������� RECT ������ ����ϴ� �Լ�
	SetRect(&rcWnd, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	RECT	rcMainView{};

	// GetClientRect : ���� viewâ�� RECT ������ ������ �Լ�
	GetClientRect(&rcMainView);

	float	fRowFrm = float(rcWnd.right - rcMainView.right);
	float	fColFrm = float(rcWnd.bottom - rcMainView.bottom);
	
	// SetWindowPos : �Ű�������� ������ ��ġ�� ũ�⸦ �����ϴ� �Լ�
	// SetWindowPos(��ġ�� �������� Z������ ���� ������(NULL�� ��� ���� ������ ����), X��ǥ, Y��ǥ, ���� ũ��, ���� ũ��, ��ġ�� �������� �ɼ�(���� ������ �����ϰڴٴ� �ɼǻ���))

	pMainFrm->SetWindowPos(nullptr, 0, 0, int(WINCX + fRowFrm), int(WINCY + fColFrm), SWP_NOZORDER);




	g_hWnd = m_hWnd;

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (FAILED(m_pGraphicDev->Initialize()))
		return;

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(TEX_SINGLE, L"../Texture/Cube.png", L"Cube")))
		return;

	m_pMyTerrain = new CMyTerrain;
	m_pMyTerrain->Initialize();
	m_pMyTerrain->Set_MainView(this);


}


// CTool131View �׸���

void CTool131View::OnDraw(CDC* /*pDC*/)
{
	CTool131Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.

	

	m_pGraphicDev->Render_Begin();


	m_pMyTerrain->Render();


	m_pGraphicDev->Render_End();

}

#pragma region �Ⱦ��� �Ⱥ�
// CTool131View �μ�
BOOL CTool131View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow(cs);
}

BOOL CTool131View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CTool131View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CTool131View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CTool131View ����

#ifdef _DEBUG
void CTool131View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTool131View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CTool131Doc* CTool131View::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTool131Doc)));
	return (CTool131Doc*)m_pDocument;
}
#endif //_DEBUG


// CTool131View �޽��� ó����


#pragma endregion �Ⱦ��� �Ⱥ�


void CTool131View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CScrollView::OnLButtonDown(nFlags, point);

	CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMiniView*		pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplitter.GetPane(0, 0));
	CMyForm*		pMyForm = dynamic_cast<CMyForm*>(pMainFrm->m_SecondSplitter.GetPane(1, 0));
	CMapTool*		pMapTool = &(pMyForm->m_MapTool);

	m_pMyTerrain->Tile_Change(D3DXVECTOR3(float(point.x + GetScrollPos(0)), float(point.y + GetScrollPos(1)), 0.f), pMapTool->m_iDrawID);




	pMiniView->Invalidate(FALSE);



	// Invalidate : ȣ�� �� �����쿡 WM_PAINT�� WM_ERASEBKGND �޼����� �߻���Ŵ.  OnDraw�Լ��� �ٽ� �ѹ� ȣ��

	//FALSE �� ��� : WM_PAINT�� �޼��� �߻�
	//true �� ��� : WM_PAINT�� WM_ERASEBKGND �� �� �߻�

	Invalidate(FALSE);
}

// ������ �������� �̿��Ͽ� �浹ó���� �ϰ� Ÿ���� �����ض�

//y = ax + b	a(����) , b(y ����)

void CTool131View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CScrollView::OnMouseMove(nFlags, point);

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		CMiniView*		pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplitter.GetPane(0, 0));
		CMyForm*		pMyForm = dynamic_cast<CMyForm*>(pMainFrm->m_SecondSplitter.GetPane(1, 0));
		CMapTool*		pMapTool = &(pMyForm->m_MapTool);

		m_pMyTerrain->Tile_Change(D3DXVECTOR3(float(point.x + GetScrollPos(0)), float(point.y + GetScrollPos(1)), 0.f), pMapTool->m_iDrawID);
		Invalidate(FALSE);

		pMiniView->Invalidate(FALSE);
	}

}
