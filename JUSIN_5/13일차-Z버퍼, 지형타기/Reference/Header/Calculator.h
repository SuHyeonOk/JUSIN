#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCalculator :	public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCalculator(const CCalculator& rhs);
	virtual ~CCalculator();

public:
	HRESULT		Ready_Calculator(void);

	_float		HeightOnTerrain(_vec3* pPos,					// 플레이어 위치값
								const _vec3* pTerrainVtxPos,	// 터레인 버텍스 위치값
								const _ulong& dwCntX,			// 어떤 삼각형인지에 따라 
								const _ulong& dwCntZ,			// 어떤 평면의 방정식을 쓰기 위해
								const _ulong& dwVtxItv = 1);	// 

public:
	virtual CComponent* Clone(void) override;
	static CCalculator*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void) override;

};

END