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

	//D3DXGetImageInfoFromFile : 저장한 이미지 파일에 관한 정보를 얻어와 구조체에 채워주는 함수
	if (FAILED(D3DXGetImageInfoFromFile(pFilePath, &(m_pTexInfo->tImgInfo))))
	{
		AfxMessageBox(pFilePath);
		
		Release();

		return E_FAIL;
	}


	// 메모리 풀

	//D3DPOOL_DEFAULT = 0, : 가장 적합한 메모리에 보관, 보통 그래픽카드 메모리
	//D3DPOOL_MANAGED = 1, : dx에 의해 자원을 관리하며 그래픽 카드 메모리를 사용하지만 ram에 백업
	//D3DPOOL_SYSTEMMEM = 2, : ram에 저장
	//D3DPOOL_SCRATCH = 3, : ram을 사용하지만 dx장치로 접근이 불가

	if (FAILED(D3DXCreateTextureFromFileEx(CDevice::Get_Instance()->Get_Device(),	// 디바이스 컴객체
		pFilePath,	// 파일의 경로
		m_pTexInfo->tImgInfo.Width,	// 텍스처 가로 사이즈
		m_pTexInfo->tImgInfo.Height,// 텍스처 세로 사이즈
		m_pTexInfo->tImgInfo.MipLevels, // 텍스처 밉레벨
		0,	// 텍스처의 용도(일반적인 텍스처는 O, D3DUSAGE_RENDERTARGET을 넣게 되면 화면 출력용 텍스처)
		m_pTexInfo->tImgInfo.Format, // 텍스처 포맷
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,	// 이미지 필터링 방식
		D3DX_DEFAULT,	// 밉맵 이미지 필터링 방식
		0,				// 제거할 색상, 0을 넣어주면 컬러키 무효화, dx에서는 알파 블랜딩을 사용하기 때문에 딱히 다른 값을 넣어주지 않음
		nullptr,		// 이미지 정보 기록
		nullptr,		// 팔레트 이미지에 대한 처리
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
