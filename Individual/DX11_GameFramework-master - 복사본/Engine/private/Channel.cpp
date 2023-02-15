#include "..\public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

/* 특정 애니메이션에서 사용되는 뼈. */
HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_pBone = pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pBone);

	/* 이뼈는 이 애니메이션을 구동하는데 있어 몇개의 구분 상태를 가지고 있는가?! */
	m_iNumKeyframes = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyframes = max(m_iNumKeyframes, pAIChannel->mNumPositionKeys);

	XMFLOAT3		vScale;
	XMFLOAT4		vRotation;
	XMFLOAT3		vPosition;

	for (_uint i = 0; i < m_iNumKeyframes; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumScalingKeys)		// 스케일 얻어오기
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(XMFLOAT3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)	// 회전 얻어오기
		{
			/*memcpy(&vScale, &pAIChannel->mRotationKeys[i].mValue, sizeof(XMFLOAT3));*/
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)	// 이동 얻어오기
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(XMFLOAT3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

/* 현재 애니메이션이 재생된 시간을 얻어온다. PlayTime */
void CChannel::Update_TransformMatrix(_double PlayTime)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지면. */
	// 보관 할 것이 없으니 이 동작에서 멈춰 있으면 된다.
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		// m_iCurrentKeyFrameIndex : 무조건 0 번째 부터 시작한다.
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)	// while 프레임 드랍에 대한 예외처리
		{
			// 마지막 키 프레임 까지 갔다면 현재 키 프레임을 하나 증가시킨다.
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourScale, vDestScale;			// 이전(Sour) 현재(Dest)
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	// XMMatrixAffineTransformation() 을 안 쓰고 스, 자, 이, 공, 부를 사용해도 된다.
	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	// 그리고 뼈에 상태에게 전달한다.
	m_pBone->Set_TransformMatrix(TransformMatrix);
}

void CChannel::Blend_TransformMatrix(_double PlayTime, _float fBlendRadio)
{
	// 기존 애니메이션 정보
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, m_pBone->Get_TransformMatrix());

	// 새롭게 구할 애니메이션 정보
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지면.*/
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	// 임의로 정한 Radio 을 넣어준다. (0.2f)
	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRadio);
	vRotation = XMQuaternionSlerp(vBaseRot, vRotation, fBlendRadio);
	vPosition = XMVectorLerp(vBasePos, vPosition, fBlendRadio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	_matrix TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CModel* pModel)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_KeyFrames.clear();
}
