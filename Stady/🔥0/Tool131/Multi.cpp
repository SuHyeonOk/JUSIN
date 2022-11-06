#include "stdafx.h"
#include "Multi.h"
#include "Device.h"


CMulti::CMulti()
{
}


CMulti::~CMulti()
{
	Release();
}

HRESULT CMulti::Insert_Texture(const TCHAR* pFilePath, const TCHAR* pStateKey /*= L""*/, const int& iCount /*= 0*/)
{
	// ����� ���̴� ���� 520����Ʈ�� �Ѿ �ȵȴ�.
	TCHAR		szFullPath[MAX_PATH] = L"";

	for (int i = 0; i < iCount; ++i)
	{
		swprintf_s(szFullPath, pFilePath, i);

		TEXINFO* pTexInfo = new TEXINFO;
		ZeroMemory(pTexInfo, sizeof(TEXINFO));

		//D3DXGetImageInfoFromFile : ������ �̹��� ���Ͽ� ���� ������ ���� ����ü�� ä���ִ� �Լ�
		if (FAILED(D3DXGetImageInfoFromFile(szFullPath, &(pTexInfo->tImgInfo))))
		{
			AfxMessageBox(pFilePath);
			
			pTexInfo->pTexture->Release();
			Safe_Delete(pTexInfo);

			return E_FAIL;
		}

		if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),	// ����̽� �İ�ü
			szFullPath,	// ������ ���
			pTexInfo->tImgInfo.Width,	// �ؽ�ó ���� ������
			pTexInfo->tImgInfo.Height,// �ؽ�ó ���� ������
			pTexInfo->tImgInfo.MipLevels, // �ؽ�ó �ӷ���
			0,	// �ؽ�ó�� �뵵(�Ϲ����� �ؽ�ó�� O, D3DUSAGE_RENDERTARGET�� �ְ� �Ǹ� ȭ�� ��¿� �ؽ�ó)
			pTexInfo->tImgInfo.Format, // �ؽ�ó ����
			D3DPOOL_MANAGED,
			D3DX_DEFAULT,	// �̹��� ���͸� ���
			D3DX_DEFAULT,	// �Ӹ� �̹��� ���͸� ���
			0,				// ������ ����, 0�� �־��ָ� �÷�Ű ��ȿȭ, dx������ ���� ������ ����ϱ� ������ ���� �ٸ� ���� �־����� ����
			nullptr,		// �̹��� ���� ���
			nullptr,		// �ȷ�Ʈ �̹����� ���� ó��
			&(pTexInfo->pTexture))))
		{
			AfxMessageBox(L"Multi Texture Failed");

			pTexInfo->pTexture->Release();
			Safe_Delete(pTexInfo);
			return E_FAIL;
		}

		m_MapMultiTex[pStateKey].push_back(pTexInfo);
	}
		
	return S_OK;
}

void CMulti::Release(void)
{
	for_each(m_MapMultiTex.begin(), m_MapMultiTex.end(), [](auto& MyPair)
	{
		for_each(MyPair.second.begin(), MyPair.second.end(), Safe_Delete<TEXINFO*>);
		MyPair.second.clear();
	});
	m_MapMultiTex.clear();
}

const TEXINFO* CMulti::Get_Texture(const TCHAR* pStateKey /*= L""*/, const int& iCount /*= 0*/)
{
	auto	iter = find_if(m_MapMultiTex.begin(), m_MapMultiTex.end(),
		[&](auto& MyPair)->bool
	{
		if (MyPair.first == pStateKey)
			return true;

		return false;
	});

	if (iter == m_MapMultiTex.end())
		return nullptr;

	return iter->second[iCount];
}
