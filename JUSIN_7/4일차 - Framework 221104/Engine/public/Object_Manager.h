#pragma once

#include "Base.h"

/* 게임내에 필요한 객체들을 내 기준(CLayer)에 따라 나누어 보관한다. */
/* 객체들을 나누어 저장하고 있는 CLayer들을 보관하는 클래스이다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);

public:
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);

	// CObject_Manager
private: /* 원형객체들을 모아놓는다. */
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;

private: /* 사본객체들을 보관하기위한 컨테이너. */
	map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;
	typedef map<const _tchar*, class CLayer*>	LAYERS;
	_uint										m_iNumLevels = 0;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END