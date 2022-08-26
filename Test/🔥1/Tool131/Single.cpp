#include "stdafx.h"
#include "Single.h"
#include "Device.h"


CSingle::CSingle() : m_pTexInfo(nullptr)
{
}


CSingle::~CSingle()
{
	Release();
}



HRESULT CSingle::Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey /*= L""*/, const int& iCount /*= 0*/)
{
	m_pTexInfo = new TEXINFO;
	ZeroMemory(m_pTexInfo, sizeof(TEXINFO));

	//D3DXGetImageInfoFromFile : ������ �̹��� ���Ͽ� ���� ������ ���� ����ü�� ä���ִ� �Լ�
	if (FAILED(D3DXGetImageInfoFromFile(pFilePath, &(m_pTexInfo->tImgInfo))))
	{
		AfxMessageBox(pFilePath);
		
		Release();

		return E_FAIL;
	}


	// �޸� Ǯ

	//D3DPOOL_DEFAULT = 0, : ���� ������ �޸𸮿� ����, ���� �׷���ī�� �޸�
	//D3DPOOL_MANAGED = 1, : dx�� ���� �ڿ��� �����ϸ� �׷��� ī�� �޸𸮸� ��������� ram�� ���
	//D3DPOOL_SYSTEMMEM = 2, : ram�� ����
	//D3DPOOL_SCRATCH = 3, : ram�� ��������� dx��ġ�� ������ �Ұ�

	if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),	// ����̽� �İ�ü
		pFilePath,	// ������ ���
		m_pTexInfo->tImgInfo.Width,	// �ؽ�ó ���� ������
		m_pTexInfo->tImgInfo.Height,// �ؽ�ó ���� ������
		m_pTexInfo->tImgInfo.MipLevels, // �ؽ�ó �ӷ���
		0,	// �ؽ�ó�� �뵵(�Ϲ����� �ؽ�ó�� O, D3DUSAGE_RENDERTARGET�� �ְ� �Ǹ� ȭ�� ��¿� �ؽ�ó)
		m_pTexInfo->tImgInfo.Format, // �ؽ�ó ����
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,	// �̹��� ���͸� ���
		D3DX_DEFAULT,	// �Ӹ� �̹��� ���͸� ���
		0,				// ������ ����, 0�� �־��ָ� �÷�Ű ��ȿȭ, dx������ ���� ������ ����ϱ� ������ ���� �ٸ� ���� �־����� ����
		nullptr,		// �̹��� ���� ���
		nullptr,		// �ȷ�Ʈ �̹����� ���� ó��
		&(m_pTexInfo->pTexture))))
	{
		AfxMessageBox(L"Single Texture Failed");
		
		Release();
		return E_FAIL;
	}


	return S_OK;
}

void CSingle::Release(void)
{
	

	m_pTexInfo->pTexture->Release();
	Safe_Delete(m_pTexInfo);
}
