// UnitTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool131.h"
#include "UnitTool.h"
#include "afxdialogex.h"


// CUnitTool ��ȭ �����Դϴ�.

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


// CUnitTool �޽��� ó�����Դϴ�.


void CUnitTool::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// UpdateData : ������ �����ϴ� �Լ�

	UpdateData(TRUE);	// ���̾�α� �ڽ��κ��� �Էµ� ������ ����

	m_strCopy = m_strTemp;


	UpdateData(FALSE);	// ������ ����� ������ ���̾�α� �ڽ��� �ݿ�
}



void CUnitTool::OnAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData(TRUE);

	UNITDATA*		pUnit = new UNITDATA;

	pUnit->strName = m_strName;
	pUnit->iHp = m_iHp;
	pUnit->iAttack = m_iAttack;

	for (int i = 0; i < 3; ++i)
	{
		// GetCheck : ���� ��ư�� üũ�Ǿ��ִ����� �Ǻ��ϴ� �Լ�
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

	//AddString : ����Ʈ �ڽ��� ���ڿ��� �߰��ϴ� �Լ�
	m_ListBox.AddString(pUnit->strName);

	m_mapUnitData.insert({pUnit->strName, pUnit});

	UpdateData(TRUE);

}


void CUnitTool::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	for_each(m_mapUnitData.begin(), m_mapUnitData.end(), CDeleteMap());
	m_mapUnitData.clear();

}


void CUnitTool::OnListBox()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CString	strFindName;

	// GetCurSel : ����Ʈ �ڽ� ��� �� ������ ���� �ε��� ��ȣ�� ��ȯ�ϴ� �Լ�
	int	iSelect = m_ListBox.GetCurSel();

	if (-1 == iSelect)
		return;

	//GetText : ���� �ε����� �ش��ϴ� ���ڿ��� ����Ʈ �ڽ��κ��� ������ �Լ�
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CUnitTool::OnSaveData()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// CFileDialog: ���� ���� Ȥ�� ���� �۾��� �ʿ��� ��ȭ ���ڸ� �����ϴ� ��ü
	
	CFileDialog		Dlg(FALSE, // TRUE�� ��� �ҷ�����, FALSE�� ��� �ٸ� �̸����� ���� ���
						L"dat", // default ���� Ȯ���ڸ�
						L"*.dat", // ��ȭ ���ڿ� ���ʿ� ǥ�õ� ���ڿ�
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // OFN_HIDEREADONLY(�б� ���� üũ �ڽ� ���� �ɼ�) / OFN_OVERWRITEPROMPT(�ߺ� ���� �� ��� �޼��� ��� �ɼ�)
						L"Data Files(*.dat) | *.dat||", // ��ȭ ���ڿ� ǥ�õ� ���� ����
						this);	// �θ� ������ �ּ�

	TCHAR	szPath[MAX_PATH] = L"";

	// GetCurrentDirectory : ���� ������Ʈ�� �ִ� ���丮 ��θ� ����(���� ���)
	GetCurrentDirectory(MAX_PATH, szPath);

	// D:\����ȯ\131��\Frame\Tool131

	// PathRemoveFileSpec : ��ü ��ο��� �����̸��� �߶��ִ� �Լ�, ��� �� �����̸��� ������ �� ������ �������� �߶󳻴� ����� ����
	PathRemoveFileSpec(szPath);
	
	// D:\����ȯ\131��\Frame\
	
	lstrcat(szPath, L"\\Data");

	// lpstrInitialDir : ��ȭ ���� â ���� �� �⺻ ��θ� �����ϴ� ��� ����
	Dlg.m_ofn.lpstrInitialDir = szPath;



	// DoModal : ��ȭ ���ڸ� ����, idok �Ǵ� idcancel�� ��ȯ
	if (IDOK == Dlg.DoModal())
	{
		// GetPathName : ���õ� ��θ� ��ȯ
		// GetString : ���� ���ڿ��� ��ȯ�ϴ� �Լ�
		CString			str = Dlg.GetPathName();
		const TCHAR*	pGetPath = str.GetString();

		HANDLE	hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD	dwByte = 0;
		DWORD	dwStrByte = 0;

		for (auto& MyPair : m_mapUnitData)
		{
			// key �� ����

			dwStrByte = sizeof(TCHAR) * (MyPair.first.GetLength() + 1);

			WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, MyPair.first, dwStrByte, &dwByte, nullptr);

			// value ����

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);


	CFileDialog		Dlg(TRUE, // TRUE�� ��� �ҷ�����, FALSE�� ��� �ٸ� �̸����� ���� ���
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
		for (auto& MyPair : m_mapUnitData)
			delete MyPair.second;

		m_mapUnitData.clear();

		//ResetContent : ������ ����Ʈ �ڽ� ����� �ʱ�ȭ
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
			// key �� �ε�

			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);

			TCHAR*	pName = new TCHAR[dwStrByte];
			ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);

			// value �ε�

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CString		strFindName = L"";

	int		iSelect = m_ListBox.GetCurSel();

	if (LB_ERR == iSelect)
		return;

	//GetText : �ε��� ������ ������ �׿� �ش��ϴ� ���ڿ��� ������ �Լ�
	m_ListBox.GetText(iSelect, strFindName);

	auto&	iter = m_mapUnitData.find(strFindName);

	if (iter == m_mapUnitData.end())
		return;


	Safe_Delete(iter->second);
	m_mapUnitData.erase(iter);

	// �ε����� �ش��ϴ� ���ڿ��� �����ϴ� �Լ�
	m_ListBox.DeleteString(iSelect);

	UpdateData(FALSE);
}


void CUnitTool::OnFindData()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData(TRUE);

	auto&	iter = m_mapUnitData.find(m_strFindName);

	if (iter == m_mapUnitData.end())
		return;

	//FindString : ���ڿ��� ��ġ�ϴ� ����Ʈ �ڽ� �� �ε����� ��ȯ, 0������ Ž��
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
