// UnitTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool131.h"
#include "UnitTool.h"
#include "afxdialogex.h"


// CUnitTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UNITTOOL, pParent)
	, m_strTemp(_T(""))
	, m_strCopy(_T(""))
	, m_strName(_T(""))
	, m_iHp(0)
	, m_iAttack(0)
	, m_strFindName(_T(""))
{

}

CUnitTool::~CUnitTool()
{
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTemp);
	DDX_Text(pDX, IDC_EDIT2, m_strCopy);
	DDX_Text(pDX, IDC_EDIT3, m_strName);
	DDX_Text(pDX, IDC_EDIT4, m_iHp);
	DDX_Text(pDX, IDC_EDIT5, m_iAttack);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_RADIO1, m_Radio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_Radio[1]);
	DDX_Control(pDX, IDC_RADIO3, m_Radio[2]);
	DDX_Control(pDX, IDC_CHECK1, m_Check[0]);
	DDX_Control(pDX, IDC_CHECK2, m_Check[1]);
	DDX_Control(pDX, IDC_CHECK3, m_Check[2]);
	DDX_Control(pDX, IDC_BUTTON3, m_Bitmap);
	DDX_Text(pDX, IDC_EDIT6, m_strFindName);
}


BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
	
	ON_BN_CLICKED(IDC_BUTTON1, &CUnitTool::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUnitTool::OnAdd)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST1, &CUnitTool::OnListBox)
	ON_BN_CLICKED(IDC_BUTTON5, &CUnitTool::OnSaveData)
	ON_BN_CLICKED(IDC_BUTTON6, &CUnitTool::OnLoadData)
	ON_BN_CLICKED(IDC_BUTTON7, &CUnitTool::OnDeleteData)
	ON_EN_CHANGE(IDC_EDIT6, &CUnitTool::OnFindData)
END_MESSAGE_MAP()


// CUnitTool 메시지 처리기입니다.


void CUnitTool::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// UpdateData : 정보를 갱신하는 함수

	UpdateData(TRUE);	// 다이얼로그 박스로부터 입력된 값들을 얻어옴

	m_strCopy = m_strTemp;


	UpdateData(FALSE);	// 변수에 저장된 값들을 다이얼로그 박스에 반영
}



void CUnitTool::OnAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	UNITDATA*		pUnit = new UNITDATA;

	pUnit->strName = m_strName;
	pUnit->iHp = m_iHp;
	pUnit->iAttack = m_iAttack;

	for (int i = 0; i < 3; ++i)
	{
		// GetCheck : 현재 버튼이 체크되어있는지를 판별하는 함수
		if (m_Radio[i].GetCheck())
		{
			pUnit->byJobIndex = i;
			break;
		}
	}

	pUnit->byItem = 0x0;

	if (m_Check[0].GetCheck())
		pUnit->byItem |= RUBY;

	if (m_Check[1].GetCheck())
		pUnit->byItem |= DIAMOND;

	if (m_Check[2].GetCheck())
		pUnit->byItem |= SAPPHIRE;

	//AddString : 리스트 박스에 문자열을 추가하는 함수
	m_ListBox.AddString(pUnit->strName);

	m_mapUnitData.insert({pUnit->strName, pUnit});

	UpdateData(TRUE);

}


void CUnitTool::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	for_each(m_mapUnitData.begin(), m_mapUnitData.end(), CDeleteMap());
	m_mapUnitData.clear();

}


void CUnitTool::OnListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString	strFindName;

	// GetCurSel : 리스트 박스 목록 중 선택한 셀의 인덱스 번호를 반환하는 함수
	int	iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	//GetText : 현재 인덱스에 해당하는 문자열을 리스트 박스로부터 얻어오는 함수
	m_ListBox.GetText(iSelect, strFindName);

	auto	iter = m_mapUnitData.find(strFindName);

	if (iter == m_mapUnitData.end())
		return;

	m_strName = iter->second->strName;
	m_iHp = iter->second->iHp;
	m_iAttack = iter->second->iAttack;

	for (int i = 0; i < 3; ++i)
		m_Radio[i].SetCheck(FALSE);

	m_Radio[iter->second->byJobIndex].SetCheck(TRUE);

	for (int i = 0; i < 3; ++i)
		m_Check[i].SetCheck(FALSE);


	if (iter->second->byItem & RUBY)
		m_Check[0].SetCheck(TRUE);

	if (iter->second->byItem & DIAMOND)
		m_Check[1].SetCheck(TRUE);

	if (iter->second->byItem & SAPPHIRE)
		m_Check[2].SetCheck(TRUE);

	
	UpdateData(FALSE);
}


BOOL CUnitTool::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_Radio[0].SetCheck(TRUE);

	HBITMAP hBitmap = (HBITMAP)LoadImage(nullptr, L"../Texture/JusinLogo1.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	m_Bitmap.SetBitmap(hBitmap);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUnitTool::OnSaveData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// CFileDialog: 파일 열기 혹은 저장 작업에 필요한 대화 상자를 생성하는 객체
	
	CFileDialog		Dlg(FALSE, // TRUE인 경우 불러오기, FALSE인 경우 다른 이름으로 저장 모드
						L"dat", // default 파일 확장자명
						L"*.dat", // 대화 상자에 최초에 표시될 문자열
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // OFN_HIDEREADONLY(읽기 전용 체크 박스 숨김 옵션) / OFN_OVERWRITEPROMPT(중복 저장 시 경고 메세지 띄움 옵션)
						L"Data Files(*.dat) | *.dat||", // 대화 상자에 표시될 파일 형식
						this);	// 부모 윈도우 주소

	TCHAR	szPath[MAX_PATH] = L"";

	// GetCurrentDirectory : 현재 프로젝트가 있는 디렉토리 경로를 얻어옴(절대 경로)
	GetCurrentDirectory(MAX_PATH, szPath);

	// D:\유준환\131기\Frame\Tool131

	// PathRemoveFileSpec : 전체 경로에서 파일이름만 잘라주는 함수, 경로 상에 파일이름이 없으면 맨 마지막 폴더명을 잘라내는 기능을 가짐
	PathRemoveFileSpec(szPath);
	
	// D:\유준환\131기\Frame\
	
	lstrcat(szPath, L"\\Data");

	// lpstrInitialDir : 대화 상자 창 생성 시 기본 경로를 저장하는 멤버 변수
	Dlg.m_ofn.lpstrInitialDir = szPath;



	// DoModal : 대화 상자를 실행, idok 또는 idcancel을 반환
	if (IDOK == Dlg.DoModal())
	{
		// GetPathName : 선택된 경로를 반환
		// GetString : 원시 문자열을 반환하는 함수
		CString			str = Dlg.GetPathName();
		const TCHAR*	pGetPath = str.GetString();

		HANDLE	hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD	dwByte = 0;
		DWORD	dwStrByte = 0;

		for (auto& MyPair : m_mapUnitData)
		{
			// key 값 저장

			dwStrByte = sizeof(TCHAR) * (MyPair.first.GetLength() + 1);

			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, MyPair.first, dwStrByte, &dwByte, nullptr);

			// value 저장

			WriteFile(hFile, &(MyPair.second->iHp), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(MyPair.second->iAttack), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(MyPair.second->byItem), sizeof(BYTE), &dwByte, nullptr);
			WriteFile(hFile, &(MyPair.second->byJobIndex), sizeof(BYTE), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}

}

/*
char	szName[245] = "asdf";
string strName = "asdfasdf";

//strName = szName;
//szName = strName;

strcpy_s(szName, strName.c_str());*/



void CUnitTool::OnLoadData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);


	CFileDialog		Dlg(TRUE, // TRUE인 경우 불러오기, FALSE인 경우 다른 이름으로 저장 모드
		L"dat", // default 파일 확장자명
		L"*.dat", // 대화 상자에 최초에 표시될 문자열
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // OFN_HIDEREADONLY(읽기 전용 체크 박스 숨김 옵션) / OFN_OVERWRITEPROMPT(중복 저장 시 경고 메세지 띄움 옵션)
		L"Data Files(*.dat) | *.dat||", // 대화 상자에 표시될 파일 형식
		this);	// 부모 윈도우 주소

	TCHAR	szPath[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szPath);
	PathRemoveFileSpec(szPath);
	
	lstrcat(szPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (IDOK == Dlg.DoModal())
	{
		for (auto& MyPair : m_mapUnitData)
			delete MyPair.second;

		m_mapUnitData.clear();

		//ResetContent : 기존의 리스트 박스 목록을 초기화
		m_ListBox.ResetContent();

		CString			str = Dlg.GetPathName();
		const TCHAR*	pGetPath = str.GetString();

		HANDLE	hFile = CreateFile(pGetPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD	dwByte = 0;
		DWORD	dwStrByte = 0;
		UNITDATA	tData{};

		while (true)
		{
			// key 값 로딩

			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);

			TCHAR*	pName = new TCHAR[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);

			// value 로드

			ReadFile(hFile, &(tData.iHp), sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &(tData.iAttack), sizeof(int), &dwByte, nullptr);
			ReadFile(hFile, &(tData.byItem), sizeof(BYTE), &dwByte, nullptr);
			ReadFile(hFile, &(tData.byJobIndex), sizeof(BYTE), &dwByte, nullptr);

			if (0 == dwByte)
			{
				delete[] pName;
				break;
			}

			UNITDATA*	pUnit = new UNITDATA;
			pUnit->strName = pName;
			delete[]pName;

			pUnit->byItem = tData.byItem;
			pUnit->byJobIndex = tData.byJobIndex;
			pUnit->iHp = tData.iHp;
			pUnit->iAttack = tData.iAttack;

			m_mapUnitData.insert({ pUnit->strName, pUnit });

			m_ListBox.AddString(pUnit->strName);
		}

		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}


void CUnitTool::OnDeleteData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString		strFindName = L"";

	int		iSelect = m_ListBox.GetCurSel();

	if (LB_ERR == iSelect)
		return;

	//GetText : 인덱스 정보를 가지고 그에 해당하는 문자열을 얻어오는 함수
	m_ListBox.GetText(iSelect, strFindName);

	auto&	iter = m_mapUnitData.find(strFindName);

	if (iter == m_mapUnitData.end())
		return;


	Safe_Delete(iter->second);
	m_mapUnitData.erase(iter);

	// 인덱스에 해당하는 문자열을 삭제하는 함수
	m_ListBox.DeleteString(iSelect);

	UpdateData(FALSE);
}


void CUnitTool::OnFindData()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	auto&	iter = m_mapUnitData.find(m_strFindName);

	if (iter == m_mapUnitData.end())
		return;

	//FindString : 문자열과 일치하는 리스트 박스 셀 인덱스를 반환, 0번부터 탐색
	int		iSelect = m_ListBox.FindString(0, m_strFindName);

	if (LB_ERR == iSelect)
		return;

	m_ListBox.SetCurSel(iSelect);

	m_strName = iter->second->strName;
	m_iHp = iter->second->iHp;
	m_iAttack = iter->second->iAttack;

	for (int i = 0; i < 3; ++i)
		m_Radio[i].SetCheck(FALSE);

	m_Radio[iter->second->byJobIndex].SetCheck(TRUE);

	for (int i = 0; i < 3; ++i)
		m_Check[i].SetCheck(FALSE);


	if (iter->second->byItem & RUBY)
		m_Check[0].SetCheck(TRUE);

	if (iter->second->byItem & DIAMOND)
		m_Check[1].SetCheck(TRUE);

	if (iter->second->byItem & SAPPHIRE)
		m_Check[2].SetCheck(TRUE);


	UpdateData(FALSE);
}
