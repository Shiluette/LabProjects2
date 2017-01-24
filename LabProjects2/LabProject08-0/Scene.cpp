#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
	//080
	m_ppShaders = NULL;

	m_nShaders = 0;
	m_pCamera = NULL;

	m_pSelectedObject = NULL;
}

CScene::~CScene()
{
}

//080
void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CTerrainShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);

	m_ppShaders[1] = new CInstancingShader(GetTerrain());
	m_ppShaders[1]->CreateShader(pd3dDevice);
	m_ppShaders[1]->BuildObjects(pd3dDevice);
}

//080
void CScene::ReleaseObjects()
{
	for (int j = 0; j < m_nShaders; j++)
	{
		m_ppShaders[j]->ReleaseObjects();
		delete m_ppShaders[j];
	}
	if (m_ppShaders) delete[] m_ppShaders;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		/*윈도우의 클라이언트 영역에서 왼쪽 마우스 버튼이 눌려지면
		마우스의 위치를 사용하여 픽킹을 처리한다. 마우스 픽킹으로
		선택된 객체가 있으면 그 객체를 비활성화한다.*/
		//if (m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam))) m_pSelectedObject->SetActive(false);
		m_pSelectedObject = PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam));
		break;
	}
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}
bool CScene::ProcessInput()
{
	return(false);
}

//080
void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}
}

//080
void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
	}
}

//080
CGameObject *CScene::PickObjectPointedByCursor(int xClient, int yClient)
{
	if (!m_pCamera) return(NULL);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;
	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if (pIntersectedObject && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	return(pNearestObject);
}

//080
CHeightMapTerrain *CScene::GetTerrain()
{
	CTerrainShader *pSceneShader = (CTerrainShader *)m_ppShaders[0];
	return(pSceneShader->GetTerrain());
}
