// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool131.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM), m_Popup(L"그냥")
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyForm::OnUnitTool)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyForm::OnMapTool)
	ON_BN_CLICKED(IDC_BUTTON8, &CMyForm::OnPopup)
	ON_BN_CLICKED(IDC_BUTTON9, &CMyForm::OnPathFind)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.


void CMyForm::OnUnitTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//GetSafeHwnd : 현재 다이얼로그의 윈도우 핸들을 반환
	if(nullptr == m_UnitTool.GetSafeHwnd())
		m_UnitTool.Create(IDD_UNITTOOL);	// 해당 id에 맞는 다이얼로그 생성

	m_UnitTool.ShowWindow(SW_SHOW);	// 창 모양으로 출력


}


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_Font.CreatePointFont(180, L"궁서");

	//GetDlgItem : 다이얼로그에 배치된 리소스를 id값에 따라 얻어오는 함수
	GetDlgItem(IDC_BUTTON1)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON4)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON8)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON9)->SetFont(&m_Font);
	
	if (nullptr == m_Popup.GetSafeHwnd())
		m_Popup.Create(0, WS_OVERLAPPEDWINDOW);

}


void CMyForm::OnMapTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_MapTool.GetSafeHwnd())
		m_MapTool.Create(IDD_MAPTOOL);	// 해당 id에 맞는 다이얼로그 생성

	m_MapTool.ShowWindow(SW_SHOW);	// 창 모양으로 출력
}


void CMyForm::OnPopup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Popup.ShowWindow(SW_SHOW);
}


void CMyForm::OnPathFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (nullptr == m_PathFind.GetSafeHwnd())
		m_PathFind.Create(IDD_PATHFIND);	// 해당 id에 맞는 다이얼로그 생성

	m_PathFind.ShowWindow(SW_SHOW);	// 창 모양으로 출력
}
