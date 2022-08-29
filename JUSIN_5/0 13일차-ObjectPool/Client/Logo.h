#pragma once
#include "Scene.h"
class CLogo :
	public CScene
{
public:
	CLogo();
	virtual ~CLogo();

public:
	CRITICAL_SECTION		Get_Crt(void) { return m_Crt; }

public:
	virtual HRESULT Ready_Scene() override;
	virtual void Update_Scene() override;
	virtual void Late_Update_Scene() override;
	virtual void Render_Scene() override;
	virtual void Release_Scene() override;

public:
	static size_t __stdcall ImgLoadThreadFunc(void* pVoid);

public:
	static CLogo*		Create(void);

private:
	CRITICAL_SECTION			m_Crt;
	HANDLE						m_hThread;

};

