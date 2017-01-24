#pragma once
//0720
#include "Shader.h"

class CScene
{
	//0720
private:

	//080
	//씬은 여러 개의 다른 쉐이더 객체들을 포함한다.
	CShader **m_ppShaders;
	 
	int m_nShaders;
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);

//0753
public:
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient);
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
private:
	CCamera *m_pCamera;
	CGameObject *m_pSelectedObject;

//08
public:
	CHeightMapTerrain *GetTerrain();
};