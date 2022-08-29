#pragma once

#include "Include.h"
#include "Device.h"

class CMainGame
{
public:
	HRESULT		Initialize(void);
	void		Update(void);
	void		Late_Update(void);
	void		Render(void);
	void		Release(void);

public:
	CMainGame();
	~CMainGame();
};

