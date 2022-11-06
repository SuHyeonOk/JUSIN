// MapTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool131.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "FileInfo.h"
#include "MainFrm.h"
#include "Tool131View.h"


// CMapTool ��ȭ �����Դϴ�.

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


// CMapTool �޽��� ó�����Դϴ�.


void CMapTool::OnListBox()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// isdigit : �Ű������� �Ѱ��ִ� ���Ϲ��ڰ� ������ �������� ������ �������� �Ǻ��ϴ� �Լ�
	//		������ ���ڷ� �Ǹ�Ǹ� 0�� �ƴ� ���� ��ȯ

		if (0 != isdigit(strFindName[i]))
			break;
	}

	// Delete : 1���ڷκ��� 2���ڱ��� ���ڸ� �����ϴ� �Լ�
	strFindName.Delete(0, i);

	m_iDrawID = _tstoi(strFindName);

	UpdateData(FALSE);
}


void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	UpdateData(TRUE);

	TCHAR		szFilePath[MAX_PATH] = L"";
	TCHAR		szFileName[MIN_STR] = L"";

	// DragQueryFile : ��ӵ� ���� ������ ������ �Լ�
	// 2��° ���� : 0xffffffff(-1)�� �����ϸ� ��ӵ� ������ ������ ��ȯ�ϴ� �ɼ�

	int	iFileCnt = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		CString	strRelative = CFileInfo::ConvertRelativePath(szFilePath);

		// PathFindFileName : ��� �� ���� �̸��� �����ϴ� �Լ�
		CString	strFileName = PathFindFileName(strRelative);

		lstrcpy(szFileName, strFileName.GetString());

		//PathRemoveExtension : ������ Ȯ���ڸ��� �����ϴ� �Լ�
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

		// GetTextExtent : ���ڿ��� ���̸� �ȼ������� ��ȯ
		size = pDC->GetTextExtent(strName);

		if (size.cx > iWidth)
			iWidth = size.cx;		
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent : ���� ��ũ���� �� �ִ� �ִ� ������ ������ �Լ�
	if (iWidth > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iWidth);

}


void CMapTool::OnSaveData()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog		Dlg(FALSE, // TRUE�� ��� �ҷ�����, FALSE�� ��� �ٸ� �̸����� ���� ���
		L"dat", // default ���� Ȯ���ڸ�
		L"*.dat", // ��ȭ ���ڿ� ���ʿ� ǥ�õ� ���ڿ�
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // OFN_HIDEREADONLY(�б� ���� üũ �ڽ� ���� �ɼ�) / OFN_OVERWRITEPROMPT(�ߺ� ���� �� ��� �޼��� ��� �ɼ�)
		L"Data Files(*.dat) | *.dat||", // ��ȭ ���ڿ� ǥ�õ� ���� ����
		this);	// �θ� ������ �ּ�

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
