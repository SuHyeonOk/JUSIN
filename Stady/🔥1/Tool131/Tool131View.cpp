
// Tool131View.cpp : CTool131View 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CTool131View 생성/소멸

CTool131View::CTool131View()
	: m_pGraphicDev(CDevice::Get_Instance())
{
	// TODO: 여기에 생성 코드를 추가합니다.

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
	//	SetScrollSizes : 스크롤 바의 사이즈를 지정하는 함수
	// MM_TEXT : 픽셀 단위로 크기를 조정하겠다는 옵션

	SetScrollSizes(MM_TEXT, CSize(TILEX * TILECX, TILEY * TILECY / 2));
	
	// AfxGetMainWnd: 현재의 메인 윈도우를 반환하는 전역 함수
	CMainFrame*		pMainFrm = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWnd{};

	// GetWindowRect : 현재 윈도우의 RECT 정보를 얻어오는 함수
	pMainFrm->GetWindowRect(&rcWnd);

	// SetRect : 지정한 매개변수로 RECT 정보를 기록하는 함수
	SetRect(&rcWnd, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	RECT	rcMainView{};

	// GetClientRect : 현재 view창의 RECT 정보를 얻어오는 함수
	GetClientRect(&rcMainView);

	float	fRowFrm = float(rcWnd.right - rcMainView.right);
	float	fColFrm = float(rcWnd.bottom - rcMainView.bottom);
	
	// SetWindowPos : 매개변수대로 윈도우 위치와 크기를 조정하는 함수
	// SetWindowPos(배치할 윈도우의 Z순서에 대한 포인터(NULL인 경우 순서 변경을 안함), X좌표, Y좌표, 가로 크기, 세로 크기, 배치할 윈도우의 옵션(현재 순서를 유지하겠다는 옵션상태))

	pMainFrm->SetWindowPos(nullptr, 0, 0, int(WINCX + fRowFrm), int(WINCY + fColFrm), SWP_NOZORDER);




	g_hWnd = m_hWnd;

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (FAILED(m_pGraphicDev->Initialize()))
		return;

	if (FAILED(CTextureMgr::Get_Instance()->Insert_Texture(TEX_SINGLE, L"../Texture/Cube.png", L"Cube")))
		return;

	m_pMyTerrain = new CMyTerrain;
	m_pMyTerrain->Initialize();
	m_pMyTerrain->Set_MainView(this);


}


// CTool131View 그리기

void CTool131View::OnDraw(CDC* /*pDC*/)
{
	CTool131Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	

	m_pGraphicDev->Render_Begin();


	m_pMyTerrain->Render();


	m_pGraphicDev->Render_End();

}

#pragma region 안쓰고 안봄
// CTool131View 인쇄
BOOL CTool131View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

BOOL CTool131View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTool131View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTool131View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CTool131View 진단

#ifdef _DEBUG
void CTool131View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CTool131View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CTool131Doc* CTool131View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTool131Doc)));
	return (CTool131Doc*)m_pDocument;
}
#endif //_DEBUG


// CTool131View 메시지 처리기


#pragma endregion 안쓰고 안봄


void CTool131View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CScrollView::OnLButtonDown(nFlags, point);

	CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CMiniView*		pMiniView = dynamic_cast<CMiniView*>(pMainFrm->m_SecondSplitter.GetPane(0, 0));
	CMyForm*		pMyForm = dynamic_cast<CMyForm*>(pMainFrm->m_SecondSplitter.GetPane(1, 0));
	CMapTool*		pMapTool = &(pMyForm->m_MapTool);

	m_pMyTerrain->Tile_Change(D3DXVECTOR3(float(point.x + GetScrollPos(0)), float(point.y + GetScrollPos(1)), 0.f), pMapTool->m_iDrawID);




	pMiniView->Invalidate(FALSE);



	// Invalidate : 호출 시 윈도우에 WM_PAINT와 WM_ERASEBKGND 메세지를 발생시킴.  OnDraw함수를 다시 한번 호출

	//FALSE 인 경우 : WM_PAINT만 메세지 발생
	//true 인 경우 : WM_PAINT와 WM_ERASEBKGND 둘 다 발생

	Invalidate(FALSE);
}

// 직선의 방정식을 이용하여 충돌처리를 하고 타일을 변경해라

//y = ax + b	a(기울기) , b(y 절편)

void CTool131View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
