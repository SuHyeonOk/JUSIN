#pragma once
#include "Component.h"

/* 객체의 월드 스페이스 상에서의 상태를 저장해준다.(m_WorldMatrix) */
/* 상태변환을 위한 인터페이스를 제공해준다.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (원점 또는 부모) 기준으로부터 얼마나 떨어져있어. */
	/* Position : 절대적인 월드공간에서의 위치. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct tagTransformDesc
	{
		/* 초당 움직여야하는 속도. */
		float		fSpeedPerSec;

		/* 초당 회전해야하는 속도. */
		float		fRotationPerSec;

		_float3		f3Pos;
	}TRANSFORMDESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}

	void Set_State(STATE eState, _fvector vState, class CNavigation* pNaviCom = nullptr);

	void Set_Pos();
	void Set_Pos(_float _vPosY) { m_WorldMatrix.m[STATE_TRANSLATION][1] = _vPosY; }
	void Set_Pos(_float3 _vPos)	{
		//_float4		vPos;
		//XMStoreFloat4(&vPos, _vPos);
		memcpy(&m_WorldMatrix.m[STATE_TRANSLATION][0], &_vPos, sizeof _vPos);
	}

	void Set_Scaled(STATE eState, _float fScale);	/* fScale값으로 길이를 변형한다. */
	void Set_Scaled(_float3 vScale);				/* fScale값으로 길이를 변형한다. */
	void Scaling(STATE eState, _float fScale);		/* fScale배수로 늘린다. */


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

	void Go_Straight(_double TimeDelta, class CNavigation* pNaviCom = nullptr);
	void Go_Straight(_double TimeDelta, _float fSpeed, class CNavigation* pNaviCom = nullptr);
	void Go_Backward(_double TimeDelta, class CNavigation* pNaviCom = nullptr);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_Up(const _double & TimeDelta);
	void Go_Down(const _double & TimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta);		/* Dynamic 어느 축을 기준으로 시간만큼 회전하는지*/
	void Rotation(_fvector vAxis, _float fRadian);		/* Static 어떤 축을 기준으로 어느 각도를 바라보는지*/

	/* 쳐다본다. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos, _bool bIsInv = false);

	/* LookAt 함수의 경우에 포인트가 하늘에 있다면, 오브젝트가 위로 기울여서 바라보게 된다.
	비행 슈팅 게임의 경우에 상관없지만, 지금처럼 바닥을 타고 다니는 오브젝트에게 이런 LookAt 함수를 사용하면 어색하게 된다.
	그래서 바라보는 포인트가 하늘에 있든 말든 위로 기울이지 않고 바라만 보게 하는 함수이다.
	예로 플레이어의 원점이 발바닥에 있다고 한다면, 몬스터가 플레이어를 바라볼 때 LookAt을 사용하면 약간 기울여서 발바닥을 보게 된다.*/
	void LookAtForLandObject(_fvector vTargetPos);

	/* 추적한다 .*/
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f, class CNavigation* pNaviCom = nullptr);
	void Speed_Chase(_fvector vTargetPos, _float fSpeed, _double TimeDelta, _float fLimit = 0.1f);
	_bool TargetPoint(_fvector vTargetPos, _double TimeDelta);

	_bool	Go_SwinUp(_double TimeDelta, _float fSpeed);
	void	Go_SwinDown(_double TimeDelta, _float fDeep);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

public: // sh
	bool		Jump(_float fHeight, _float fSpeed, _double TimeDelta);
	//void		RandomJump(_float iRandHeight, _float fSpeed, _float fminusHeight, _double TimeDelta, _bool bOneDir = false, _bool bTurn = true);
	_vector		LinearVector(_fvector vSour, _fvector vDest, _float fAngle);
	void		PlayerMove(_fvector vNewLook, const _double & TimeDelta);

private:	
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

private:
	_float		m_fAngle = 0.0f;	// 플레이어가 이동할 Look 을 천천히 계산한다.

	// Jump()
	_bool		m_bJump = false;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


END