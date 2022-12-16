#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	void	Reset_KeyFrameIndex()	{ m_iCurrentKeyFrameIndex = 0; }
	_uint	Get_Keyframes()			{ return m_iCurrentKeyFrameIndex;}

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void	Update_TransformMatrix(_double PlayTime);

public:
	void	Blend_TransformMatrix(_double PlayTime, _float fBlendRadio);

private:
	char				m_szName[MAX_PATH] = "";
	class CBone*		m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;
};

END