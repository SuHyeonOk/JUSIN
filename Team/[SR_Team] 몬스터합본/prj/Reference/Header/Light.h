#pragma once

#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

// 조명에 대한 반사를 통해 색상을 판단. 조명도 재질도 반사속성을 가지고 있다.
// 조명은 외부에서 생성하여 내부에서 관리.
// 즉, 누가 조명을 소유하느냐의 문제가 있다.
// 조명 클래스는 on/off 역할만 한다.
class ENGINE_DLL CLight : public CBase
{
public:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight();

public:
	HRESULT Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	D3DLIGHT9			m_tLightInfo; // D3DLIGHT9 : 조명과 관련된 구조체 (하단 참조)
	_uint				m_iIndex;	// 몇번인덱스에 저장되는 조명인지

public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const D3DLIGHT9* pLightInfo,
		const _uint& iIndex);

private:
	virtual void	Free(void);

};

END

/*
D3DLIGHT9 : 조명과 관련된 구조체
	- diffuse(난반사)/specular(정반사)/ambient(환경반사)
	- position(점, 스포트라이트 조명은 반드시 가져야 함
	- direction(방향성 조면은 반드시) - 둘 다 월드 좌표
	- range: position으로부터의 거리
	- falloff: 딱히 안쓰임
	- attenuation : 감쇄 3단계 .보편적으로 0번만 쓴다.
			1) 연산량이 많아서 2) 사람 눈에 편차가 크지 않아서
	- theta : 스포트라이트 광원의 안쪽 각도
	- phi : 스포트 라이트 광원의 바깥 각도

* 조명 
- 난반사(diffuse) 	: 규칙적인 방향성 없이 반사
					일반적인 색상(어디서 봐도 그 색깔)
- 정반사(specular)	: 입사각과 반사각이 동일한 반사
					매끄러운 재질에 빛이 입사할 때 하이라이트 표현
					기본적으로 CPU가 담당. 연산량이 많고 속도가 떨어진다.
- 환경반사(ambient)	: 빛은 DSA 속성을 모두 가지고 있음.
					물체의 재질도 DSA속성을 가지고 있음.
					반사연산이란 각 속성끼리 계산하는 것.
					환경반사란, 환경에 의해 보이는 색상
ex) 반사된 빛에 의해 어떤 물체를 보게된다면 원래 색상과는 조금 다를것
* 조명
- Directional 	: 방향성 광원		// 실제 태양은 점 광원이지만, 게임내 적용할 땐 어색.
- Point 		: 점 광원			// 점기준 반경내 연산. 횃불. 가장 많이 쓰는 광원.
- Spot			: 스프라이트 광원	// 콘 형 조명. 랜턴. 후레쉬. 가로등. 감쇄(끝으로 갈수록 약해짐) => 연산량이 많아 점 광원으로 대체하기도 함.


*/
