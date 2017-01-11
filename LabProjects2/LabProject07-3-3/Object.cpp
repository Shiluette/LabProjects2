#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

//0733
//‘A' 키가 눌리면 이 변수의 값은 true가 된다. 다시 한번 'A' 키가 눌리면 false가 된다.
bool gbAutomaticMoveRotate = false;

//0733
CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_d3dxmtxTransform);
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_ppMeshes = NULL;
	m_nMeshes = 0;
	m_pChild = m_pSibling = m_pParent = NULL;
	m_bIsVisible = false;
	m_bActive = true;
	m_nReferences = 0;
}

//0733
CGameObject::~CGameObject()
{
	for (int i = 0; i < m_nMeshes; i++) if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
	if (m_ppMeshes) delete[] m_ppMeshes;
	if (m_pSibling) delete m_pSibling;
	if (m_pChild) delete m_pChild;
}

void CGameObject::AddRef()
{
	m_nReferences++;
}

void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

//0733
void CGameObject::SetMesh(CMesh *pMesh)
{
	if (pMesh)
	{
		CMesh **ppMeshes = new CMesh*[m_nMeshes + 1];
		for (int i = 0; i < m_nMeshes; i++) ppMeshes[i] = m_ppMeshes[i];
		ppMeshes[m_nMeshes] = pMesh;
		delete[] m_ppMeshes;
		m_ppMeshes = ppMeshes;
		m_nMeshes++;
		pMesh->AddRef();
	}
}

//0733
void CGameObject::SetChild(CGameObject *pChild)
{
	//객체의 첫 번째 자식 객체가 존재하면 pChild를 m_pChild 객체의 마지막 형제 객체로 설정한다.
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
	//pChild 객체의 부모 객체를 현재의 객체로 설정한다.
	if (pChild) pChild->m_pParent = this;
}

//0733
void CGameObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	/*부모 객체가 있으면 그 부모 객체의 월드 변환 행렬을 현재 객체의 로컬 변환 행렬에 곱하여 월드 변환 행렬에 저장한다.*/
	if (pd3dxmtxParent) D3DXMatrixMultiply(&m_d3dxmtxWorld, &m_d3dxmtxTransform, pd3dxmtxParent);

	/*객체의 형제 객체를 애니메이션한다. 형제 객체의 애니메이션은 부모 객체의 월드 변환 행렬을 사용한다.*/
	if (m_pSibling) m_pSibling->Animate(fTimeElapsed, pd3dxmtxParent);
	/*객체의 첫 번째 자식 객체를 애니메이션한다. 자식 객체의 애니메이션은 현재 객체의 월드 변환 행렬을 사용한다.*/
	if (m_pChild) m_pChild->Animate(fTimeElapsed, &m_d3dxmtxWorld);
}

//0733
/*게임 객체의 위치를 로컬 좌표계 또는 월드 좌표계를 사용하여 설정한다. bd3dxmtxTransform가 참이면 게임 객체들의 계층구조에서 부모에 상대적인 위치를 설정한다.*/
void CGameObject::SetPosition(float x, float y, float z, bool bd3dxmtxTransform)
{
	if (bd3dxmtxTransform)
	{
		m_d3dxmtxTransform._41 = x;
		m_d3dxmtxTransform._42 = y;
		m_d3dxmtxTransform._43 = z;
	}
	else
	{
		m_d3dxmtxWorld._41 = x;
		m_d3dxmtxWorld._42 = y;
		m_d3dxmtxWorld._43 = z;
	}
}

//0733
void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition, bool bd3dxmtxTransform)
{
	SetPosition(d3dxvPosition.x, d3dxvPosition.y, d3dxvPosition.z, bd3dxmtxTransform);
}

//0733
void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	/*게임 객체가 보인다면 메쉬를 렌더링한다. m_bIsVisible 변수의 값은 CGameObject::IsVisible() 함수에서 결정된다.*/
	if (m_bIsVisible)
	{
		OnPrepareRender();
		//월드 변환 행렬을 매핑하여 쉐이더의 상수 버퍼 변수로 전달한다.
		CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
		//현재 게임 객체의 모든 메쉬를 렌더링한다.
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i]->Render(pd3dDeviceContext);
	}

	//게임 객체의 형제 객체를 렌더링한다. 
	if (m_pSibling) m_pSibling->Render(pd3dDeviceContext, pCamera);
	//게임 객체의 첫 번째 자식 객체를 렌더링한다. 
	if (m_pChild) m_pChild->Render(pd3dDeviceContext, pCamera);
}

//0733
D3DXVECTOR3 CGameObject::GetPosition(bool bd3dxmtxTransform)
{
	return((bd3dxmtxTransform) ? D3DXVECTOR3(m_d3dxmtxTransform._41, m_d3dxmtxTransform._42, m_d3dxmtxTransform._43) : D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

//0733
D3DXVECTOR3 CGameObject::GetLook(bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvLookAt = (bd3dxmtxTransform) ? D3DXVECTOR3(m_d3dxmtxTransform._31, m_d3dxmtxTransform._32, m_d3dxmtxTransform._33) : D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

//0733
D3DXVECTOR3 CGameObject::GetUp(bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvUp = (bd3dxmtxTransform) ? D3DXVECTOR3(m_d3dxmtxTransform._21, m_d3dxmtxTransform._22, m_d3dxmtxTransform._23) : D3DXVECTOR3(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

//0733
D3DXVECTOR3 CGameObject::GetRight(bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvRight = (bd3dxmtxTransform) ? D3DXVECTOR3(m_d3dxmtxTransform._11, m_d3dxmtxTransform._12, m_d3dxmtxTransform._13) : D3DXVECTOR3(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

//0733
void CGameObject::MoveStrafe(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	//게임 객체의 로컬 좌표계의 x-축 방향으로 움직인다.
	D3DXVECTOR3 d3dxvRight = GetRight(bd3dxmtxTransform);
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::MoveUp(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	D3DXVECTOR3 d3dxvUp = GetUp(bd3dxmtxTransform);
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::MoveForward(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	D3DXVECTOR3 d3dxvLookAt = GetLook(bd3dxmtxTransform);
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::MoveStrafeW(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	//월드 좌표계의 x-축 방향으로 움직인다.
	D3DXVECTOR3 d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::MoveUpW(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	D3DXVECTOR3 d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::MoveForwardW(float fDistance, bool bd3dxmtxTransform)
{
	D3DXVECTOR3 d3dxvPosition = GetPosition(bd3dxmtxTransform);
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition, bd3dxmtxTransform);
}

//0733
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll, bool bd3dxmtxTransform)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	if (bd3dxmtxTransform)
		m_d3dxmtxTransform = mtxRotate * m_d3dxmtxTransform;
	else
		m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

//0733
void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle, bool bd3dxmtxTransform)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	if (bd3dxmtxTransform)
		m_d3dxmtxTransform = mtxRotate * m_d3dxmtxTransform;
	else
		m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

//0733
void CGameObject::Revolve(float fPitch, float fYaw, float fRoll, bool bd3dxmtxTransform)
{
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	if (bd3dxmtxTransform)
		m_d3dxmtxTransform = m_d3dxmtxTransform * mtxRotate;
	else
		m_d3dxmtxWorld = m_d3dxmtxWorld * mtxRotate;
}

//0733
AABB CGameObject::GetBoundingCube()
{
	//모든 메쉬의 바운딩 박스를 합하여 월드 변환 행렬로 변환한다.
	AABB bcBoundingCube = m_ppMeshes[0]->GetBoundingCube();
	for (int i = 1; i < m_nMeshes; i++) bcBoundingCube.Merge(&m_ppMeshes[i]->GetBoundingCube());
	bcBoundingCube.Update(&m_d3dxmtxWorld);
	return(bcBoundingCube);
}

//0733
bool CGameObject::IsVisible(CCamera *pCamera)
{
	if (!pCamera) return(m_bIsVisible = true);

	bool bIsVisible = false;
		//현재 게임 객체의 바운딩 박스가 카메라의 절두체와 겹치는 가를 검사한다.
	AABB bcBoundingCube = GetBoundingCube();
	bIsVisible = m_bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);

	//현재 게임 객체의 형제 객체가 카메라에 보이는 가를 검사한다.
	if (m_pSibling) bIsVisible |= m_pSibling->IsVisible(pCamera);
	//현재 게임 객체의 자식 객체가 카메라에 보이는 가를 검사한다.
	if (m_pChild) bIsVisible |= m_pChild->IsVisible(pCamera);

	//현재 게임 객체의 계층 구조 중 하나라도 카메라에 보이면 그 객체가 카메라에 보이는 것이다.
	return(bIsVisible);
}

//0733
//게임 객체(계층 구조)가 키보드 메시지를 처리할 수 있도록 한다.
bool CGameObject::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	bool bChild = false, bSibling = false;
	if (m_pSibling) bSibling = m_pSibling->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	if (m_pChild) bChild = m_pChild->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return(bChild | bSibling);
}

//072
CRotatingObject::CRotatingObject()
{
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

//0733
void CRotatingObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	CGameObject::Rotate(&m_d3dxvRotationAxis, m_fRotationSpeed * fTimeElapsed);
}

//0733
void CRotatingObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CGameObject::Render(pd3dDeviceContext, pCamera);
}

//073
CRevolvingObject::CRevolvingObject()
{
	m_d3dxvRevolutionAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

//073
CRevolvingObject::~CRevolvingObject()
{
}

//073
void CRevolvingObject::Animate(float fTimeElapsed)
{
	//공전을 나타내기 위해 회전 행렬을 오른쪽에 곱한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRevolutionAxis, (float)D3DXToRadian(m_fRevolutionSpeed * fTimeElapsed));
	m_d3dxmtxWorld = m_d3dxmtxWorld * mtxRotate;
}

//0733
CCannonBallObject::CCannonBallObject()
{
	m_fRotationSpeed = 90.0f;
	m_d3dxvRotationAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	m_fMovingSpeed = 70.0f;
	m_fEffectiveRange = m_fMovingSpeed * 5.0f;
	m_fMovingDistance = 0.0f;

	//포탄은 초기에 비활성화된다.
	m_bActive = false;
}

//0733
CCannonBallObject::~CCannonBallObject()
{
}

//0733
void CCannonBallObject::SetActive(bool bActive)
{
	m_fMovingDistance = 0.0f;
	m_bActive = bActive;
}

//0733
void CCannonBallObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	CRotatingObject::Animate(fTimeElapsed, pd3dxmtxParent);
	//포탄을 이동 속력에 따라 움직이고 움직인 거리를 전체 이동 거리에 더한다.
	MoveStrafe(m_fMovingSpeed * fTimeElapsed, false);
	m_fMovingDistance += m_fMovingSpeed * fTimeElapsed;
	//포탄의 전체 이동 거리가 유효 사거리 보다 크면 포탄을 비활성화한다.
	if (m_fMovingDistance > m_fEffectiveRange) SetActive(false);
}

//0733
CGunObject::CGunObject(ID3D11Device *pd3dDevice)
{
	m_fRotationAngle = 0.0f;
	m_fRotationSpeed = 30.0f;

	CSphereMesh *pBallMesh = new CSphereMesh(pd3dDevice, 2.5f, 10, 10, D3DCOLOR_XRGB(0, 0, 128));

	m_nBalls = 10;
	m_ppBalls = new CCannonBallObject*[m_nBalls];
	for (int i = 0; i < m_nBalls; i++)
	{
		m_ppBalls[i] = new CCannonBallObject();
		m_ppBalls[i]->SetMesh(pBallMesh);
		m_ppBalls[i]->AddRef();
	}
}

//0733
CGunObject::~CGunObject()
{
	for (int i = 0; i < m_nBalls; i++) if (m_ppBalls[i]) m_ppBalls[i]->Release();
	if (m_ppBalls) delete[] m_ppBalls;
}

//0733
void CGunObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	/*키보드의 'A' 키가 눌려지면 포신은 위로 45도, 아래로 10도까지 계속 회전한다. 위 아래의 회전은 z-축을 중심으로 회전하는 것이다. 이것은 포신의 모델 메쉬를 원통으로 만들고 원통의 방향을 x-축으로 설정하였기 때문이다.*/
	if (::gbAutomaticMoveRotate)
	{
		if ((m_fRotationAngle <= -10.0f) || (m_fRotationAngle >= 45.0f)) m_fRotationSpeed = -m_fRotationSpeed;
		float fRotateAngle = m_fRotationSpeed * fTimeElapsed;
		m_fRotationAngle += fRotateAngle;
		Rotate(0.0f, 0.0f, fRotateAngle, true);
	}

	CGameObject::Animate(fTimeElapsed, pd3dxmtxParent);

	//포탄이 활성화되면 포탄을 애니메이션한다.
	for (int i = 0; i < m_nBalls; i++)
	{
		if (m_ppBalls[i]->IsActive()) m_ppBalls[i]->Animate(fTimeElapsed, NULL);
	}
}

//0733
void CGunObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CGameObject::Render(pd3dDeviceContext, pCamera);

	//포탄이 활성화되고 카메라에 보인다면 포탄을 렌더링한다.
	for (int i = 0; i < m_nBalls; i++)
	{
		if (m_ppBalls[i]->IsActive() && m_ppBalls[i]->IsVisible(pCamera)) m_ppBalls[i]->Render(pd3dDeviceContext, pCamera);
	}
}

//0733
//포탄을 발사하기 위하여 키보드 메시지를 포신 객체에서 처리한다.
bool CGunObject::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			//‘Space' 키가 눌려지면 포탄을 현재 포신의 방향으로 발사한다.
		case VK_SPACE: //Fire 
		{

			int nBallToShoot = m_nBalls;
			for (int i = 0; i < m_nBalls; i++)
			{
				if (!m_ppBalls[i]->IsActive())
				{
					nBallToShoot = i;
					break;
				}
			}
			if (nBallToShoot < m_nBalls)
			{
				//사용할 수 있는 포탄 객체가 있으면 그 포탄 객체를 활성화한다.
				m_ppBalls[nBallToShoot]->SetActive(true);
				/*포탄 객체의 월드 변환 행렬을 포신 객체의 월드 변환 행렬로 설정한다. 그러면 포탄의 위치와 방향이 포신 객체의 위치와 방향과 같아진다. 포신(원통) 모델의 방향은 x-축 방향이므로 포탄을 발사하기 위하여 포탄을 로컬 x-축 방향으로 움직이면 된다.*/
				m_ppBalls[nBallToShoot]->SetTransform(m_d3dxmtxWorld, false);
			}
			return(true);
			break;
		}
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

//0733
CTurretObject::CTurretObject()
{
	m_fRotationSpeed = 45.0f;
}

//0733
CTurretObject::~CTurretObject()
{
}

//0733
void CTurretObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	//키보드의 'A' 키가 눌려지면 포탑을 y-축을 중심으로 회전한다.
	if (::gbAutomaticMoveRotate) Rotate(0.0f, m_fRotationSpeed * fTimeElapsed, 0.0f, true);
	CGameObject::Animate(fTimeElapsed, pd3dxmtxParent);
}

//0733
CTankObject::CTankObject()
{
	m_fMovingSpeed = 1.0f;
}

//0733
CTankObject::~CTankObject()
{
}

//0733
void CTankObject::Animate(float fTimeElapsed, D3DXMATRIX *pd3dxmtxParent)
{
	//키보드의 'A' 키가 눌려지면 탱크를 로컬 y-축을 중심으로 회전하면서 월드 좌표계의 z-축 방향으로 반복적으로 앞-뒤로 이동한다.
	if (::gbAutomaticMoveRotate)
	{
		static float sfBodyMovement = 0.0f, sfMovement = 5.0f;
		if ((sfBodyMovement >= 80.0f) || (sfBodyMovement <= -30.0f)) sfMovement = -sfMovement;
		float fMovement = sfMovement * fTimeElapsed;
		sfBodyMovement += fMovement;
		MoveForwardW(fMovement, false);
		Rotate(0.0f, 15.0f * fTimeElapsed, 0.0f, false);
	}

	CGameObject::Animate(fTimeElapsed, pd3dxmtxParent);
}
