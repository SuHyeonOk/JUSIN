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
	// 경로의 길이는 절대 520바이트를 넘어선 안된다.
	TCHAR		szFullPath[MAX_PATH] = L"";

	for (int i = 0; i < iCount; ++i)
	{
		swprintf_s(szFullPath, pFilePath, i);

		TEXINFO* pTexInfo = new TEXINFO;
		ZeroMemory(pTexInfo, sizeof(TEXINFO));

		//D3DXGetImageInfoFromFile : 저장한 이미지 파일에 관한 정보를 얻어와 구조체에 채워주는 함수
		if (FAILED(D3DXGetImageInfoFromFile(szFullPath, &(pTexInfo->tImgInfo))))
		{
			AfxMessageBox(pFilePath);
			
			pTexInfo->pTexture->Release();
			Safe_Delete(pTexInfo);

			return E_FAIL;
		}

		if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),	// 디바이스 컴객체
			szFullPath,	// 파일의 경로
			pTexInfo->tImgInfo.Width,	// 텍스처 가로 사이즈
			pTexInfo->tImgInfo.Height,// 텍스처 세로 사이즈
			pTexInfo->tImgInfo.MipLevels, // 텍스처 밉레벨
			0,	// 텍스처의 용도(일반적인 텍스처는 O, D3DUSAGE_RENDERTARGET을 넣게 되면 화면 출력용 텍스처)
			pTexInfo->tImgInfo.Format, // 텍스처 포맷
			D3DPOOL_MANAGED,
			D3DX_DEFAULT,	// 이미지 필터링 방식
			D3DX_DEFAULT,	// 밉맵 이미지 필터링 방식
			0,				// 제거할 색상, 0을 넣어주면 컬러키 무효화, dx에서는 알파 블랜딩을 사용하기 때문에 딱히 다른 값을 넣어주지 않음
			nullptr,		// 이미지 정보 기록
			nullptr,		// 팔레트 이미지에 대한 처리
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
