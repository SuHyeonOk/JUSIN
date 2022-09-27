#pragma once

#include "Base.h"
#include "Engine_Include.h"

BEGIN(Engine)

// 컴포넌트 패턴 : "기능들을 부품화" 하여 필요할 때 "가져다 쓰겠다"는 디자인 패턴이다.
// 무엇을 컴포넌트화 시킬 것인가에 대한 고민이 필요하다.
// manangement > Scene > Layer > Object > Component

class ENGINE_DLL CComponent : public CBase
{
protected:
	explicit CComponent();
	explicit CComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent();

public:
	virtual _int			Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void			LateUpdate_Component(void) {}

protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	_bool					m_bClone;

public:
	virtual CComponent*	Clone(void)PURE;	// 프로토타입 패턴
											// 원본 하나를 만들어 보관하고, 사본을 복사생성해 필요할 때 가져다 쓴다.
											// ex) return new CRcCol(*this); => 컴포넌트화 했다.
	virtual void		Free(void);
};

END