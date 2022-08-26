// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool131.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "FileInfo.h"
#include "MainFrm.h"
#include "Tool131View.h"


// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOL, pParent)
{

}

CMapTool::~CMapTool()
{
	for_each(m_MapPngImg.begin(), m_MapPngImg.end(), [](auto& MyPair)
	{
		MyPair.second->Destroy();
		Safe_Delete<CImage*>(MyPair.second);
	});

	m_MapPngImg.clear();

}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnListBox)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnSaveData)
END_MESSAGE_MAP()


// CMapTool 메시지 처리기입니다.


void CMapTool::OnListBox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CString		strFindName;

	int	iSelect = m_ListBox.GetCurSel();

	if (LB_ERR == iSelect)
		return;

	m_ListBox.GetText(iSelect, strFindName);

	auto iter = m_MapPngImg.find(strFindName);

	if (iter == m_MapPngImg.end())
		return;

	m_Picture.SetBitmap(*(iter->second));

	int		i = 0;

	for (; i < strFindName.GetLength(); ++i)
	{
	// isdigit : 매개변수로 넘겨주는 단일문자가 글자형 글자인지 숫자형 글자인지 판별하는 함수
	//		숫자형 글자로 판명되면 0이 아닌 값을 반환

		if (0 != isdigit(strFindName[i]))
			break;
	}

	// Delete : 1인자로부터 2인자까지 문자를 삭제하는 함수
	strFindName.Delete(0, i);

	m_iDrawID = _tstoi(strFindName);

	UpdateData(FALSE);
}


void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	UpdateData(TRUE);

	TCHAR		szFilePath[MAX_PATH] = L"";
	TCHAR		szFileName[MIN_STR] = L"";

	// DragQueryFile : 드롭된 파일 정보를 얻어오는 함수
	// 2번째 인자 : 0xffffffff(-1)로 지정하면 드롭된 파일의 개수를 반환하는 옵션

	int	iFileCnt = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		CString	strRelative = CFileInfo::ConvertRelativePath(szFilePath);

		// PathFindFileName : 경로 중 파일 이름만 추출하는 함수
		CString	strFileName = PathFindFileName(strRelative);

		lstrcpy(szFileName, strFileName.GetString());

		//PathRemoveExtension : 파일의 확장자명을 제거하는 함수
		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_MapPngImg.find(szFileName);

		if (iter == m_MapPngImg.end())
		{
			CImage*		pPngImage = new CImage;
			pPngImage->Load(strRelative);

			m_MapPngImg.insert({ strFileName, pPngImage });
			m_ListBox.AddString(strFileName);

		}
	}

	HorizontalScroll();

	UpdateData(FALSE);

	CDialog::OnDropFiles(hDropInfo);
}

void CMapTool::HorizontalScroll(void)
{
	CString		strName;
	CSize		size;

	int	iWidth = 0;

	CDC*		pDC = m_ListBox.GetDC();

	for (int i = 0; i < m_ListBox.GetCount(); ++i)
	{
		m_ListBox.GetText(i, strName);

		// GetTextExtent : 문자열의 길이를 픽셀단위로 변환
		size = pDC->GetTextExtent(strName);

		if (size.cx > iWidth)
			iWidth = size.cx;		
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent : 가로 스크롤할 수 있는 최대 범위를 얻어오는 함수
	if (iWidth > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iWidth);

}


void CMapTool::OnSaveData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog		Dlg(FALSE, // TRUE인 경우 불러오기, FALSE인 경우 다른 이름으로 저장 모드
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
		CString			str = Dlg.GetPathName();
		const TCHAR*	pGetPath = str.GetString();

		HANDLE	hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		CMainFrame*		pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
		CTool131View*	pMainView = dynamic_cast<CTool131View*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
		CMyTerrain*		pMyTerrain = pMainView->m_pMyTerrain;
		vector<TILE*>&	vecTile = pMyTerrain->Get_Tile();

		DWORD dwByte = 0;

		for (auto& iter : vecTile)
			WriteFile(hFile, iter, sizeof(TILE), &dwByte, nullptr);		

		CloseHandle(hFile);
	}

}
