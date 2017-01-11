#pragma once

#include "Camera.h"
#include "Mesh.h"

//0733
extern bool gbAutomaticMoveRotate;

//0733
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

protected:
	//게임 객체는 메쉬들의 리스트를 갖는다. 
	CMesh **m_ppMeshes;
	int m_nMeshes;
	//게임 객체의 첫 번째 자식 객체에 대한 포인터이다. 
	CGameObject *m_pChild;
	//게임 객체의 형제 객체에 대한 포인터이다. 
	CGameObject *m_pSibling;
	//게임 객체의 부모 객체에 대한 포인터이다. 
	CGameObject *m_pParent;

	//게임 객체가 카메라에 보이는 상태를 나타내는 변수이다.
	bool m_bIsVisible;
	//게임 객체의 활성화 상태를 설정한다. 객체가 비활성화되면 객체는 움직이지 않게 된다.
	bool m_bActive;

	//게임 객체의 로컬 변환 행렬이다. 게임 객체들의 계층구조에서 부모에 상대적인 변환을 나타낸다.
	D3DXMATRIX  m_d3dxmtxTransform;

public:
	//게임 객체의 월드 변환 행렬이다. 게임 객체를 렌더링하려면 월드 변환 행렬이 필요하다.
	D3DXMATRIX  m_d3dxmtxWorld;

public:
	virtual void SetMesh(CMesh *pMesh);
	//게임 객체의 자식 객체를 설정한다. 
	virtual void SetChild(CGameObject *pChild);
	//게임 객체의 부모 객체를 반환한다. 
	CGameObject *GetParent() { return(m_pParent); }

	void SetTransform(D3DXMATRIX d3dxmtxTransform, bool bd3dxmtxTransform = false) { if (bd3dxmtxTransform) m_d3dxmtxTransform = d3dxmtxTransform; else m_d3dxmtxWorld = d3dxmtxTransform; }
	D3DXMATRIX GetTransform(bool bd3dxmtxTransform = false) { return((bd3dxmtxTransform) ? m_d3dxmtxTransform : m_d3dxmtxWorld); }

	void SetPosition(float x, float y, float z, bool bd3dxmtxTransform = false);
	void SetPosition(D3DXVECTOR3 d3dxvPosition, bool bd3dxmtxTransform = false);

	void MoveStrafe(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveUp(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveForward(float fDistance = 1.0f, bool bd3dxmtxTransform = false);

	//게임 객체를 월드 좌표계의 x-축, y-축, z-축 방향으로 이동한다. 
	void MoveStrafeW(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveUpW(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveForwardW(float fDistance = 1.0f, bool bd3dxmtxTransform = false);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f, bool bd3dxmtxTransform = false);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle, bool bd3dxmtxTransform = false);
	void Revolve(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f, bool bd3dxmtxTransform = false);
	void Revolve(D3DXVECTOR3 *pd3dxvAxis, float fAngle, bool bd3dxmtxTransform = false);

	D3DXVECTOR3 GetPosition(bool bd3dxmtxTransform = false);
	D3DXVECTOR3 GetLook(bool bd3dxmtxTransform = false);
	D3DXVECTOR3 GetUp(bool bd3dxmtxTransform = false);
	D3DXVECTOR3 GetRight(bool bd3dxmtxTransform = false);

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);
	virtual void OnPrepareRender() { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	AABB GetBoundingCube();
	bool IsVisible(CCamera *pCamera = NULL);

	virtual void SetActive(bool bActive = true) { m_bActive = bActive; }
	bool IsActive() { return(m_bActive); }
};

//0733
class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

protected:
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;
};

//073
class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject();
	virtual ~CRevolvingObject();

	virtual void Animate(float fTimeElapsed);

private:
	// 공전 회전축과 회전 속력을 나타낸다.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// 공전 속력을 설정한다.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// 공전을 위한 회전축을 설정한다.
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }
};

//0733
class CCannonBallObject : public CRotatingObject
{
public:
	CCannonBallObject();
	virtual ~CCannonBallObject();

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);

	void SetMovingSpeed(float fMovingSpeed) { m_fMovingSpeed = fMovingSpeed; }
	virtual void SetActive(bool bActive = true);

private:
	//포탄 객체의 이동 속력(1초 동안 움직이는 거리)를 나타낸다. 
	float m_fMovingSpeed;
	//포탄 객체의 유효사거리를 나타낸다. 
	float m_fEffectiveRange;
	//포탄 객체가 움직인 거리를 나타낸다. 
	float m_fMovingDistance;
};

//0733
class CGunObject : public CGameObject
{
public:
	CGunObject(ID3D11Device *pd3dDevice);
	virtual ~CGunObject();

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	CCannonBallObject **m_ppBalls;
	int m_nBalls;
	int m_nBallToShoot;

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetRotationAngle(float fRotationAngle) { m_fRotationAngle = fRotationAngle; }
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }

private:
	/*포신 객체의 현재 회전 각도를 나타낸다. 포신은 위-아래 방향(y-축)으로 정해진 각도 내에서만 회전한다.*/
	float	m_fRotationAngle;
	//포신 객체의 회전 속력(1초 동안 회전하는 각도)를 나타낸다. 
	float	m_fRotationSpeed;
};

//0733
class CTurretObject : public CGameObject
{
public:
	CTurretObject();
	virtual ~CTurretObject();

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);

	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }

private:
	//포탑 객체의 회전 속력(1초 동안 회전하는 각도)를 나타낸다. 포탑은 y-축으로 360도 회전할 수 있다. 
	float m_fRotationSpeed;
};

//0733
class CTankObject : public CGameObject
{
public:
	CTankObject();
	virtual ~CTankObject();

	virtual void Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent = NULL);

	void SetMovingSpeed(float fMovingSpeed) { m_fMovingSpeed = fMovingSpeed; }

private:
	//탱크 몸체의 이동 속력(1초 동안 이동하는 거리)을 나타낸다. 
	float m_fMovingSpeed;
};