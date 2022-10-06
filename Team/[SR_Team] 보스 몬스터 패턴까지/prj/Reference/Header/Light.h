#pragma once

#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

// ���� ���� �ݻ縦 ���� ������ �Ǵ�. ���� ������ �ݻ�Ӽ��� ������ �ִ�.
// ������ �ܺο��� �����Ͽ� ���ο��� ����.
// ��, ���� ������ �����ϴ����� ������ �ִ�.
// ���� Ŭ������ on/off ���Ҹ� �Ѵ�.
class ENGINE_DLL CLight : public CBase
{
public:
	explicit CLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLight();

public:
	HRESULT Ready_Light(const D3DLIGHT9* pLightInfo, const _uint& iIndex);

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	D3DLIGHT9			m_tLightInfo; // D3DLIGHT9 : ����� ���õ� ����ü (�ϴ� ����)
	_uint				m_iIndex;	// ����ε����� ����Ǵ� ��������

public:
	static CLight*	Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const D3DLIGHT9* pLightInfo,
		const _uint& iIndex);

private:
	virtual void	Free(void);

};

END

/*
D3DLIGHT9 : ����� ���õ� ����ü
	- diffuse(���ݻ�)/specular(���ݻ�)/ambient(ȯ��ݻ�)
	- position(��, ����Ʈ����Ʈ ������ �ݵ�� ������ ��
	- direction(���⼺ ������ �ݵ��) - �� �� ���� ��ǥ
	- range: position���κ����� �Ÿ�
	- falloff: ���� �Ⱦ���
	- attenuation : ���� 3�ܰ� .���������� 0���� ����.
			1) ���귮�� ���Ƽ� 2) ��� ���� ������ ũ�� �ʾƼ�
	- theta : ����Ʈ����Ʈ ������ ���� ����
	- phi : ����Ʈ ����Ʈ ������ �ٱ� ����

* ���� 
- ���ݻ�(diffuse) 	: ��Ģ���� ���⼺ ���� �ݻ�
					�Ϲ����� ����(��� ���� �� ����)
- ���ݻ�(specular)	: �Ի簢�� �ݻ簢�� ������ �ݻ�
					�Ų����� ������ ���� �Ի��� �� ���̶���Ʈ ǥ��
					�⺻������ CPU�� ���. ���귮�� ���� �ӵ��� ��������.
- ȯ��ݻ�(ambient)	: ���� DSA �Ӽ��� ��� ������ ����.
					��ü�� ������ DSA�Ӽ��� ������ ����.
					�ݻ翬���̶� �� �Ӽ����� ����ϴ� ��.
					ȯ��ݻ��, ȯ�濡 ���� ���̴� ����
ex) �ݻ�� ���� ���� � ��ü�� ���Եȴٸ� ���� ������� ���� �ٸ���
* ����
- Directional 	: ���⼺ ����		// ���� �¾��� �� ����������, ���ӳ� ������ �� ���.
- Point 		: �� ����			// ������ �ݰ泻 ����. ȶ��. ���� ���� ���� ����.
- Spot			: ��������Ʈ ����	// �� �� ����. ����. �ķ���. ���ε�. ����(������ ������ ������) => ���귮�� ���� �� �������� ��ü�ϱ⵵ ��.


*/
