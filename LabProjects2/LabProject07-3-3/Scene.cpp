#include "stdafx.h"
#include "Scene.h"

//0720
CScene::CScene()
{
	m_pShaders = NULL;
	m_nShaders = 0;
}

CScene::~CScene()
{
}

//0720
void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nShaders = 1;
	m_pShaders = new CSceneShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice);
	m_pShaders[0].BuildObjects(pd3dDevice);
}

//0720
void CScene::ReleaseObjects()
{
	for (int j = 0; j < m_nShaders; j++) m_pShaders[j].ReleaseObjects();
	if (m_pShaders) delete[] m_pShaders;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

//0733
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(m_pShaders[0].OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam));
}

bool CScene::ProcessInput()
{
	return(false);
}

//0720
void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
}

//0720
void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dDeviceContext, pCamera);
	}
}
