#pragma once
#include "Component.h"

/* ��ü�� ���� �����̽� �󿡼��� ���¸� �������ش�.(m_WorldMatrix) */
/* ���º�ȯ�� ���� �������̽��� �������ش�.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (���� �Ǵ� �θ�) �������κ��� �󸶳� �������־�. */
	/* Position : �������� ������������� ��ġ. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct tagTransformDesc
	{
		/* �ʴ� ���������ϴ� �ӵ�. */
		float		fSpeedPerSec;

		/* �ʴ� ȸ���ؾ��ϴ� �ӵ�. */
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

	void Set_State(STATE eState, _fvector vState) {
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);			
	}

	void Set_Pos();
	void Set_Pos(_float _vPosY) { m_WorldMatrix.m[STATE_TRANSLATION][1] = _vPosY; }
	void Set_Pos(_float3 _vPos)	{
		//_float4		vPos;
		//XMStoreFloat4(&vPos, _vPos);
		memcpy(&m_WorldMatrix.m[STATE_TRANSLATION][0], &_vPos, sizeof _vPos);
	}

	void Set_Scaled(STATE eState, _float fScale);	/* fScale������ ���̸� �����Ѵ�. */
	void Set_Scaled(_float3 vScale);				/* fScale������ ���̸� �����Ѵ�. */
	void Scaling(STATE eState, _float fScale);		/* fScale����� �ø���. */


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

	void Go_Straight(_double TimeDelta);
	void Go_Diagonal(_double TimeDelta, _float fSpeed = 1.f);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta);		/* Dynamic ��� ���� �������� �ð���ŭ ȸ���ϴ���*/
	void Rotation(_fvector vAxis, _float fRadian);		/* Static � ���� �������� ��� ������ �ٶ󺸴���*/

	/* �Ĵٺ���. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);

	/* LookAt �Լ��� ��쿡 ����Ʈ�� �ϴÿ� �ִٸ�, ������Ʈ�� ���� ��￩�� �ٶ󺸰� �ȴ�.
	���� ���� ������ ��쿡 ���������, ����ó�� �ٴ��� Ÿ�� �ٴϴ� ������Ʈ���� �̷� LookAt �Լ��� ����ϸ� ����ϰ� �ȴ�.
	�׷��� �ٶ󺸴� ����Ʈ�� �ϴÿ� �ֵ� ���� ���� ������� �ʰ� �ٶ� ���� �ϴ� �Լ��̴�.
	���� �÷��̾��� ������ �߹ٴڿ� �ִٰ� �Ѵٸ�, ���Ͱ� �÷��̾ �ٶ� �� LookAt�� ����ϸ� �ణ ��￩�� �߹ٴ��� ���� �ȴ�.*/
	void LookAtForLandObject(_fvector vTargetPos);

	/* �����Ѵ� .*/
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

public: // sh
	bool		Jump(_float fHeight, _float fSpeed, _double TimeDelta);
	void		RandomJump(_int iRandHeight, _float fSpeed, _float fminusHeight, _double TimeDelta, _bool bOneDir = false);



private:	
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

private:
	// Jump()
	_bool		m_bJump = false;

	// RandomJump()
	// ū ���� �� ���� ���� 3��
	_bool		m_bBigJump = false;
	_float		m_fSmallJump = 0.0f;
	// ���� �� ȸ��
	_bool		m_bRotation = false; //
	// ���� �� ���� �� ���� ����
	_bool		m_bOneDir = false;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END