#pragma once

#include "Include.h"

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

public:
	static CString	ConvertRelativePath(CString strFullPath);

	// 폴더의 각종 경로 정보를 추출하는 함수
	static void	DirInfoExtraction(const wstring& wstrPath, list<IMGPATH*>& rPathList);

	// 폴더 안에 이미지가 몇장있는지 계산하여 반환하는 함수
	static int	DirFileCount(const wstring& wstrPath);

};

