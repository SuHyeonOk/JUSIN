#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}


CFileInfo::~CFileInfo()
{
}

CString CFileInfo::ConvertRelativePath(CString strFullPath)
{
	// 상대 경로를 저장할 배열
	TCHAR	szRelativePath[MAX_PATH] = L"";

	// 절대 경로를 저장할 배열
	TCHAR	szCurDir[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szCurDir);

	// PathRelativePathTo : 2인자에서 4인자로 가는 상대 경로를 구해서 1인자에 넣어주는 함수, 단, 같은 드라이브 안에서만 가능

	PathRelativePathTo(szRelativePath, 
					szCurDir,
					FILE_ATTRIBUTE_DIRECTORY,
					strFullPath.GetString(), 
					FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);

}

void CFileInfo::DirInfoExtraction(const wstring & wstrPath, list<IMGPATH*>& rPathList)
{
	// CFileFind : mfc에서 제공하는 파일 및 경로 제어 클래스
	CFileFind		Find;

	wstring		wstrFilePath = wstrPath + L"\\*.*";


	// FindFile : 주어진 경로에 파일의 유무를 판단하는 함수
	// 유무를 확인하고 실제 파일에 접근하고 싶을 경우, FindNextFile함수를 무조건 호출해야 한다.
	// FindNextFile 함수 호출 시, 여러 개 파일 중 가장 첫 번째 파일에 접근할 수 있다.
	BOOL	bContinue = Find.FindFile(wstrFilePath.c_str());

	while (bContinue)
	{
		// FindNextFile : 동일 경로 안에서 다음 번에 있는 파일을 찾는 함수, 더이상 찾을 파일이 없으면 0을 반환
		bContinue = Find.FindNextFile();

		// 찾은 파일 이름이 .이거나 ..인 경우 파일 탐색에서 건너띄기 위한 예외 코드
		if(Find.IsDots())
			continue;

		else if (Find.IsDirectory())
		{
			DirInfoExtraction(wstring(Find.GetFilePath()), rPathList);
		}
		else
		{
			// 찾은 파일이 숨겨져 있는 시스템 파일인 경우
			if(Find.IsSystem())
				continue;

			IMGPATH*		pImgPath = new IMGPATH;
			TCHAR			szPathBuf[MAX_PATH] = L"";

			lstrcpy(szPathBuf, Find.GetFilePath().GetString());

			// 현재 경로에서 "파일명.확장자"를 잘라냄
			PathRemoveFileSpec(szPathBuf);

			// 폴더 안에 몇 장의 이미지가 있는지 계산
			pImgPath->iCount = CFileInfo::DirFileCount(szPathBuf);

			// BossMultiAttack0
			// GetFileTitle : 파일 이름만 얻어오는 함수(확장자는 얻어오지 않음)
			wstring	wstrTextureName = Find.GetFileTitle().GetString();

			//BossMultiAttack%d.png
			//substr(시작, 끝) : 시작에서 끝에 해당하는 문자열을 얻어오는 함수
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";

			TCHAR	szBuf[MAX_PATH] = L"";
			lstrcpy(szBuf, Find.GetFilePath().GetString());
			PathRemoveFileSpec(szBuf);

			// PathCombine(결과값, 결합할 경로, 결합할 문자) : 2인자와 3인자 사이에 \\가 자동으로 삽입된채 1인자에 저장
			PathCombine(szBuf, szBuf, wstrTextureName.c_str());

			//..\Frame\Texture\Stage\Effect\BossMultiAttack\\BossMultiAttack%d.png
			// 완성된 경로를 상대 경로로 전환
			pImgPath->wstrPath = ConvertRelativePath(szBuf);

			//..\Frame\Texture\Stage\Effect\BossMultiAttack

			PathRemoveFileSpec(szBuf);

			//PathFindFileName : 파일명을 찾거나 가장 마지막 폴더명을 찾아내는 함수
			pImgPath->wstrStateKey = PathFindFileName(szBuf);

			//..\Frame\Texture\Stage\Effect
			PathRemoveFileSpec(szBuf);
			pImgPath->wstrObjKey = PathFindFileName(szBuf);

			rPathList.push_back(pImgPath);

			bContinue = 0;
		}
	}

}

int CFileInfo::DirFileCount(const wstring & wstrPath)
{
	wstring		wstrFilePath = wstrPath + L"\\*.*";

	CFileFind		Find;
	BOOL			bContinue = Find.FindFile(wstrFilePath.c_str());

	int	iFileCount = 0;

	while (bContinue)
	{
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		if (Find.IsSystem())
			continue;


		++iFileCount;
	}

	return iFileCount;
}
