#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void	Update_Bones(_double TimeDelta, _bool bepetition);

public:
	_bool	Get_Finished() { return m_isFinished; }

	void	Reset_Channel();
	void	Reset_Repetition() { m_bRepetition = true; }

private:
	char								m_szName[MAX_PATH];
	/* �ִϸ��̼��� ����ϴµ� �ɸ��� ��ü �ð�. */
	_double								m_Duration = 0.f;

	/* �ʴ� ����ؾ��� �ִϸ��̼��� �ӵ��̴�. */
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;

	/* �� �ִϸ��̼��� ����ϱ����� �����ؾ��ϴ� ����. */
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;

	_bool								m_bRepetition = true;	// sh �⺻������ �ݺ��� true �Ѵ�.

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END