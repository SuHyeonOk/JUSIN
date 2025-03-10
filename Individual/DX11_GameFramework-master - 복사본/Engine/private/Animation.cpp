#include "..\public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
	: m_isLooping(true)
{
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;

	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션 구동하는데 필요한 뼈대의 개수다.  */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*		pAINodeAnim = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pAINodeAnim, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Reset_Channel()
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Reset_KeyFrameIndex();

	m_PlayTime = 0.0f;
}

_int CAnimation::Get_Keyframes()
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
		return m_Channels[i]->Get_Keyframes();

	return -1;
}

void CAnimation::Update_Bones(_double TimeDelta, _bool bRepetition)
{
	if (true == m_isFinished && false == m_isLooping)
		return;

	m_PlayTime += m_TickPerSecond * TimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;

		if (!bRepetition)
			return;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)					// 애니메이션이 끝나면
			m_Channels[i]->Reset_KeyFrameIndex();	// 프레임을 처음으로 초기화

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}

	if (true == m_isFinished)
		m_isFinished = false;
}

void CAnimation::Update_Bones_Blend(_double TimeDelta, _float fBlendRatio)
{
	if (true == m_isFinished && false == m_isLooping)
		return;

	m_PlayTime += m_TickPerSecond * TimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		// 임으로 사이에 만들어준 블랜딩할 행렬로 재생한다.
		m_Channels[i]->Blend_TransformMatrix(m_PlayTime, fBlendRatio);
	}

	if (true == m_isFinished)
		m_isFinished = false;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

