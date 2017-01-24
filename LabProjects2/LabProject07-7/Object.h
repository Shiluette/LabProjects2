#pragma once

#include "Camera.h"
#include "Mesh.h"

class CGameObject
{
private:
	int m_nReferences;

protected:
	D3DXMATRIX m_d3dxmtxWorld;
	CMesh *m_pMesh;

public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef();
	void Release();

	D3DXMATRIX GetmtxWorld(void) {
		return m_d3dxmtxWorld;
	};

	virtual void SetMesh(CMesh *pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//07100
	//��ü�� ��ġ�� �����Ѵ�.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	//075
	CMesh *GetMesh() { return(m_pMesh); }

//072
public:
	//���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� x-��, y-��, z-�� �������� ȸ���Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	//��ü�� ��ġ, ���� x-��, y-��, z-�� ���� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	//��ü�� �������ϱ� ���� ȣ��Ǵ� �Լ��̴�.
	virtual void OnPrepareRender() { }

	//0730
	bool IsVisible(CCamera *pCamera = NULL);

//0753
private:
	bool m_bActive;
public:
	void SetActive(bool bActive = false) { m_bActive = bActive; }

	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);


};

class CRotatingObject : public CGameObject
{
protected:
	//���� �ӵ��� ȸ���� ���͸� ��Ÿ���� ��� ������ �����Ѵ�.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//���� �ӵ��� ȸ���� ���͸� �����ϴ� �Լ��̴�.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
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

