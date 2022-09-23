#pragma once

#include "Base.h"
#include "Engine_Include.h"
// ��ƼŬ �ý���
// �ý��� �� ��� ��ƼŬ�� ������ �ִ� ���� Ư���� ����
// ��ƼŬ ����, ���÷���, �Ҹ�, ���� ���� ����

BEGIN(Engine)

class ENGINE_DLL CPSystem : public CBase
{
	//DECLARE_SINGLETON(CPSystem)

private:
	explicit CPSystem();
	virtual ~CPSystem();

public:									
	virtual _bool	init(IDirect3DDevice9* pGraphicDev, const _tchar * texFileName); // D3D ��ġ �������� �ʱ�ȭ �۾�
	virtual void	reset();									// �ý��� �� ��� ��ƼŬ �Ӽ� ����
	virtual void	resetParticle(Attribute* attribute) = 0;	// �� ��ƼŬ�� �Ӽ��� ����
	virtual void	addParticle();								// ��ƼŬ�� �߰�
	virtual void	update(_float fTimeDelta) = 0;				// ��ƼŬ ����
	
	virtual void	preRender();		// �������� �ռ� ������ �ʱ� ���� ����
	virtual void	render();			// ������
	virtual void	postRender();		// Ư�� ��ƼŬ �ý����� �������� �� �ִ� ���� ���¸� �����Ҷ� �̿�

public:
	bool isEmpty();	// ���� �ý��ۿ� ��ƼŬ�� ������ True
	bool isDead();	// �ý��� �� ��� ��ƼŬ�� ������� True

protected:
	virtual void removeDeadParticles();	// ���� ��ƼŬ�� ����Ʈ���� ����

public:
	virtual void Free(void);

protected:
	IDirect3DDevice9*			m_pGarphicDev;
	_vec3						m_origin;			// �ý����� ��õ. ��ƼŬ�� ���۵Ǵ� ��
	//BoundingBox					m_boundingBox;		// ��ƼŬ�� �̵��� �� �ִ� ���� ����
	_float						m_fEmitRate;		// �ý��ۿ� ���ο� ��ƼŬ�� �߰��Ǵ� ����. (����/s)
	_float						m_fSize;			// �ý��� �� ��� ��ƼŬ�� ũ��
	IDirect3DTexture9*			m_Tex;
	IDirect3DVertexBuffer9*		m_vb;
	list<Attribute>				m_particles;		// �ý��� �� ��ƼŬ �Ӽ� ����Ʈ
													// ��ƼŬ ����/����/���� �� �� ����Ʈ�� �̿�
													// ��ƼŬ�� �׸� �غ� �Ϸ�Ǹ� ����Ʈ ��� �Ϻθ�
													// ���ؽ� ���۷� �����ϰ�, ��ƼŬ ����� ������ ��ħ.
													// ��� ��ƼŬ�� �׸������� �� ������ �ݺ�.
	_int						m_maxParticles;		// �־��� �ð����� �ý����� ���� �� �ִ� �ִ� ��ƼŬ ��

	_ulong						m_vbSize;			// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ ��
													// ���� ��ƼŬ �ý��� �� ��ƼŬ ���ʹ� ������
	_ulong						m_vbOffset;			// �������� �̿��
	_ulong						m_vbBatchSize;		// �������� �̿��

};

END
