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
	// ��� ��θ� ������ �迭
	TCHAR	szRelativePath[MAX_PATH] = L"";

	// ���� ��θ� ������ �迭
	TCHAR	szCurDir[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szCurDir);

	// PathRelativePathTo : 2���ڿ��� 4���ڷ� ���� ��� ��θ� ���ؼ� 1���ڿ� �־��ִ� �Լ�, ��, ���� ����̺� �ȿ����� ����

	PathRelativePathTo(szRelativePath, 
					szCurDir,
					FILE_ATTRIBUTE_DIRECTORY,
					strFullPath.GetString(), 
					FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);

}

void CFileInfo::DirInfoExtraction(const wstring & wstrPath, list<IMGPATH*>& rPathList)
{
	// CFileFind : mfc���� �����ϴ� ���� �� ��� ���� Ŭ����
	CFileFind		Find;

	wstring		wstrFilePath = wstrPath + L"\\*.*";


	// FindFile : �־��� ��ο� ������ ������ �Ǵ��ϴ� �Լ�
	// ������ Ȯ���ϰ� ���� ���Ͽ� �����ϰ� ���� ���, FindNextFile�Լ��� ������ ȣ���ؾ� �Ѵ�.
	// FindNextFile �Լ� ȣ�� ��, ���� �� ���� �� ���� ù ��° ���Ͽ� ������ �� �ִ�.
	BOOL	bContinue = Find.FindFile(wstrFilePath.c_str());

	while (bContinue)
	{
		// FindNextFile : ���� ��� �ȿ��� ���� ���� �ִ� ������ ã�� �Լ�, ���̻� ã�� ������ ������ 0�� ��ȯ
		bContinue = Find.FindNextFile();

		// ã�� ���� �̸��� .�̰ų� ..�� ��� ���� Ž������ �ǳʶ�� ���� ���� �ڵ�
		if(Find.IsDots())
			continue;

		else if (Find.IsDirectory())
		{
			DirInfoExtraction(wstring(Find.GetFilePath()), rPathList);
		}
		else
		{
			// ã�� ������ ������ �ִ� �ý��� ������ ���
			if(Find.IsSystem())
				continue;

			IMGPATH*		pImgPath = new IMGPATH;
			TCHAR			szPathBuf[MAX_PATH] = L"";

			lstrcpy(szPathBuf, Find.GetFilePath().GetString());

			// ���� ��ο��� "���ϸ�.Ȯ����"�� �߶�
			PathRemoveFileSpec(szPathBuf);

			// ���� �ȿ� �� ���� �̹����� �ִ��� ���
			pImgPath->iCount = CFileInfo::DirFileCount(szPathBuf);

			// BossMultiAttack0
			// GetFileTitle : ���� �̸��� ������ �Լ�(Ȯ���ڴ� ������ ����)
			wstring	wstrTextureName = Find.GetFileTitle().GetString();

			//BossMultiAttack%d.png
			//substr(����, ��) : ���ۿ��� ���� �ش��ϴ� ���ڿ��� ������ �Լ�
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";

			TCHAR	szBuf[MAX_PATH] = L"";
			lstrcpy(szBuf, Find.GetFilePath().GetString());
			PathRemoveFileSpec(szBuf);

			// PathCombine(�����, ������ ���, ������ ����) : 2���ڿ� 3���� ���̿� \\�� �ڵ����� ���Ե�ä 1���ڿ� ����
			PathCombine(szBuf, szBuf, wstrTextureName.c_str());

			//..\Frame\Texture\Stage\Effect\BossMultiAttack\\BossMultiAttack%d.png
			// �ϼ��� ��θ� ��� ��η� ��ȯ
			pImgPath->wstrPath = ConvertRelativePath(szBuf);

			//..\Frame\Texture\Stage\Effect\BossMultiAttack

			PathRemoveFileSpec(szBuf);

			//PathFindFileName : ���ϸ��� ã�ų� ���� ������ �������� ã�Ƴ��� �Լ�
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
