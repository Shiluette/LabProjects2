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
	//���� ��ü�� �޽����� ����Ʈ�� ���´�. 
	CMesh **m_ppMeshes;
	int m_nMeshes;
	//���� ��ü�� ù ��° �ڽ� ��ü�� ���� �������̴�. 
	CGameObject *m_pChild;
	//���� ��ü�� ���� ��ü�� ���� �������̴�. 
	CGameObject *m_pSibling;
	//���� ��ü�� �θ� ��ü�� ���� �������̴�. 
	CGameObject *m_pParent;

	//���� ��ü�� ī�޶� ���̴� ���¸� ��Ÿ���� �����̴�.
	bool m_bIsVisible;
	//���� ��ü�� Ȱ��ȭ ���¸� �����Ѵ�. ��ü�� ��Ȱ��ȭ�Ǹ� ��ü�� �������� �ʰ� �ȴ�.
	bool m_bActive;

	//���� ��ü�� ���� ��ȯ ����̴�. ���� ��ü���� ������������ �θ� ������� ��ȯ�� ��Ÿ����.
	D3DXMATRIX  m_d3dxmtxTransform;

public:
	//���� ��ü�� ���� ��ȯ ����̴�. ���� ��ü�� �������Ϸ��� ���� ��ȯ ����� �ʿ��ϴ�.
	D3DXMATRIX  m_d3dxmtxWorld;

public:
	virtual void SetMesh(CMesh *pMesh);
	//���� ��ü�� �ڽ� ��ü�� �����Ѵ�. 
	virtual void SetChild(CGameObject *pChild);
	//���� ��ü�� �θ� ��ü�� ��ȯ�Ѵ�. 
	CGameObject *GetParent() { return(m_pParent); }

	void SetTransform(D3DXMATRIX d3dxmtxTransform, bool bd3dxmtxTransform = false) { if (bd3dxmtxTransform) m_d3dxmtxTransform = d3dxmtxTransform; else m_d3dxmtxWorld = d3dxmtxTransform; }
	D3DXMATRIX GetTransform(bool bd3dxmtxTransform = false) { return((bd3dxmtxTransform) ? m_d3dxmtxTransform : m_d3dxmtxWorld); }

	void SetPosition(float x, float y, float z, bool bd3dxmtxTransform = false);
	void SetPosition(D3DXVECTOR3 d3dxvPosition, bool bd3dxmtxTransform = false);

	void MoveStrafe(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveUp(float fDistance = 1.0f, bool bd3dxmtxTransform = false);
	void MoveForward(float fDistance = 1.0f, bool bd3dxmtxTransform = false);

	//���� ��ü�� ���� ��ǥ���� x-��, y-��, z-�� �������� �̵��Ѵ�. 
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
	// ���� ȸ����� ȸ�� �ӷ��� ��Ÿ����.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// ���� �ӷ��� �����Ѵ�.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// ������ ���� ȸ������ �����Ѵ�.
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
	//��ź ��ü�� �̵� �ӷ�(1�� ���� �����̴� �Ÿ�)�� ��Ÿ����. 
	float m_fMovingSpeed;
	//��ź ��ü�� ��ȿ��Ÿ��� ��Ÿ����. 
	float m_fEffectiveRange;
	//��ź ��ü�� ������ �Ÿ��� ��Ÿ����. 
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
	/*���� ��ü�� ���� ȸ�� ������ ��Ÿ����. ������ ��-�Ʒ� ����(y-��)���� ������ ���� �������� ȸ���Ѵ�.*/
	float	m_fRotationAngle;
	//���� ��ü�� ȸ�� �ӷ�(1�� ���� ȸ���ϴ� ����)�� ��Ÿ����. 
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
	//��ž ��ü�� ȸ�� �ӷ�(1�� ���� ȸ���ϴ� ����)�� ��Ÿ����. ��ž�� y-������ 360�� ȸ���� �� �ִ�. 
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
	//��ũ ��ü�� �̵� �ӷ�(1�� ���� �̵��ϴ� �Ÿ�)�� ��Ÿ����. 
	float m_fMovingSpeed;
};