#include "stdafx.h"
#include "Player.h"

//08
CPlayer::CPlayer(int nMeshes) : CGameObject(nMeshes)
{
	m_pCamera = NULL;

	m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_d3dxvLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_d3dxvVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_d3dxvGravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	//�÷��̾��� ���� ī�޶��� UpdateShaderVariables() ��� �Լ��� ȣ���Ѵ�.
	if (m_pCamera) m_pCamera->UpdateShaderVariables(pd3dDeviceContext);
}

//072
/*�÷��̾��� ��ġ�� �����ϴ� �Լ��̴�. �÷��̾��� ��ġ�� �⺻������ ����ڰ� �÷��̾ �̵��ϱ� ���� Ű���带 ���� �� ����ȴ�. �÷��̾��� �̵� ����(dwDirection)�� ���� �÷��̾ fDistance ��ŭ �̵��Ѵ�.*/
void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		D3DXVECTOR3 d3dxvShift = D3DXVECTOR3(0, 0, 0);
		//ȭ��ǥ Ű ���衯�� ������ ���� z-�� �������� �̵�(����)�Ѵ�. ���顯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_FORWARD) d3dxvShift += m_d3dxvLook * fDistance;
		if (dwDirection & DIR_BACKWARD) d3dxvShift -= m_d3dxvLook * fDistance;
		//ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_RIGHT) d3dxvShift += m_d3dxvRight * fDistance;
		if (dwDirection & DIR_LEFT) d3dxvShift -= m_d3dxvRight * fDistance;
		//��Page Up���� ������ ���� y-�� �������� �̵��Ѵ�. ��Page Down���� ������ �ݴ� �������� �̵��Ѵ�.
		if (dwDirection & DIR_UP) d3dxvShift += m_d3dxvUp * fDistance;
		if (dwDirection & DIR_DOWN) d3dxvShift -= m_d3dxvUp * fDistance;

		//�÷��̾ ���� ��ġ ���Ϳ��� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		Move(d3dxvShift, bUpdateVelocity);
	}
}

//072
void CPlayer::Move(const D3DXVECTOR3& d3dxvShift, bool bUpdateVelocity)
{
	//bUpdateVelocity�� ���̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� �����Ѵ�.
	if (bUpdateVelocity)
	{
		m_d3dxvVelocity += d3dxvShift;
	}
	else
	{
		//�÷��̾ ���� ��ġ ���Ϳ��� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		D3DXVECTOR3 d3dxvPosition = m_d3dxvPosition + d3dxvShift;
		m_d3dxvPosition = d3dxvPosition;
		//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ��ġ�� d3dxvShift ���� ��ŭ �̵��Ѵ�.
		m_pCamera->Move(d3dxvShift);
	}
}

//072
//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�.
void CPlayer::Rotate(float x, float y, float z)
{
	D3DXMATRIX mtxRotate;
	DWORD nCurrentCameraMode = m_pCamera->GetMode();

	//1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������.
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		/*���� x-���� �߽����� ȸ���ϴ� ���� ������ �յڷ� ���̴� ���ۿ� �ش��Ѵ�. �׷��Ƿ� x-���� �߽����� ȸ���ϴ� ������ -89.0~+89.0�� ���̷� �����Ѵ�. x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ� x��ŭ ȸ���� ���� Pitch�� +89�� ���� ũ�ų� -89�� ���� ������ m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.*/
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		//���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����.
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		/*���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�. z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ� z��ŭ ȸ���� ���� m_fRoll�� +20�� ���� ũ�ų� -20������ ������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.*/
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�.
		m_pCamera->Rotate(x, y, z);

		/*�÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��. �÷��̾��� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��. �⺻������ Up ���͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.*/
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		/*�����̽�-�� ī�޶󿡼� �÷��̾��� ȸ���� ȸ�� ������ ������ ����. �׸��� ��� ���� �߽����� ȸ���� �� �� �ִ�.*/
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRight, (float)D3DXToRadian(x));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
		}
		if (y != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvUp, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&m_d3dxvLook, &m_d3dxvLook, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
		if (z != 0.0f)
		{
			D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvLook, (float)D3DXToRadian(z));
			D3DXVec3TransformNormal(&m_d3dxvUp, &m_d3dxvUp, &mtxRotate);
			D3DXVec3TransformNormal(&m_d3dxvRight, &m_d3dxvRight, &mtxRotate);
		}
	}

	/*ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ� z-��(LookAt ����)�� �������� �Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.*/
	D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
	D3DXVec3Cross(&m_d3dxvRight, &m_d3dxvUp, &m_d3dxvLook);
	D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	D3DXVec3Normalize(&m_d3dxvUp, &m_d3dxvUp);
}

//072
void CPlayer::Update(float fTimeElapsed)
{
	/*�÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ���Ѵ�. �߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ��̴�.*/
	m_d3dxvVelocity += m_d3dxvGravity * fTimeElapsed;

	/*�÷��̾��� �ӵ� ������ XZ-������ ũ�⸦ ���Ѵ�. �̰��� XZ-����� �ִ� �ӷº��� ũ�� �ӵ� ������ x�� z-���� ������ �����Ѵ�.*/
	float fLength = sqrtf(m_d3dxvVelocity.x * m_d3dxvVelocity.x + m_d3dxvVelocity.z * m_d3dxvVelocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > fMaxVelocityXZ)
	{
		m_d3dxvVelocity.x *= (fMaxVelocityXZ / fLength);
		m_d3dxvVelocity.z *= (fMaxVelocityXZ / fLength);
	}

	/*�÷��̾��� �ӵ� ������ Y-������ ũ�⸦ ���Ѵ�. �̰��� Y �� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ������ �����Ѵ�.*/
	fLength = sqrtf(m_d3dxvVelocity.y * m_d3dxvVelocity.y);
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	if (fLength > fMaxVelocityY) m_d3dxvVelocity.y *= (fMaxVelocityY / fLength);

	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�). 
	Move(m_d3dxvVelocity, false);

	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. ���� ���, �÷��̾��� ��ġ�� ����Ǿ����� �÷��̾� ��ü���� ����(Terrain)�� ������ ����. �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ���� �ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ� ������ �� �ִ�.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdated(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� ī�޶��� ���¸� �����Ѵ�.
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_d3dxvPosition, fTimeElapsed);
	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) OnCameraUpdated(fTimeElapsed);
	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�.
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_d3dxvPosition);
	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�.
	m_pCamera->RegenerateViewMatrix();

	/*�÷��̾��� �ӵ� ���Ͱ� ������ ������ ������ �Ǿ�� �Ѵٸ� ���� ���͸� �����Ѵ�. �ӵ� ������ �ݴ� ���� ���͸� ���ϰ� ���� ���ͷ� �����. ���� ����� �ð��� ����ϵ��� �Ͽ� �������� ���Ѵ�. ���� ���Ϳ� �������� ���Ͽ� ���� ���͸� ���Ѵ�. �ӵ� ���Ϳ� ���� ���͸� ���Ͽ� �ӵ� ���͸� ���δ�. �������� �ӷº��� ũ�� �ӷ��� 0�� �� ���̴�.*/
	D3DXVECTOR3 d3dxvDeceleration = -m_d3dxvVelocity;
	D3DXVec3Normalize(&d3dxvDeceleration, &d3dxvDeceleration);
	fLength = D3DXVec3Length(&m_d3dxvVelocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_d3dxvVelocity += d3dxvDeceleration * fDeceleration;
}

//072
CCamera *CPlayer::OnChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�.
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	/*���� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� Up ���͸� ������ǥ���� y-�� ���� ����(0, 1, 0)�� �ǵ��� �Ѵ�. ��, �ȹٷ� ������ �Ѵ�. �׸��� �����̽�-�� ī�޶��� ��� �÷��̾��� �̵����� ������ ����. Ư��, y-�� ������ �������� �����Ӵ�. �׷��Ƿ� �÷��̾��� ��ġ�� ����(��ġ ������ y-��ǥ�� 0���� ũ��)�� �� �� �ִ�. �̶� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ��ġ�� ������ �Ǿ�� �Ѵ�. �׷��Ƿ� �÷��̾��� Right ���Ϳ� Look ������ y ���� 0���� �����. ���� �÷��̾��� Right ���Ϳ� Look ���ʹ� �������Ͱ� �ƴϹǷ� ����ȭ�Ѵ�.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_d3dxvRight.y = 0.0f;
		m_d3dxvLook.y = 0.0f;
		D3DXVec3Normalize(&m_d3dxvRight, &m_d3dxvRight);
		D3DXVec3Normalize(&m_d3dxvLook, &m_d3dxvLook);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		/*Look ���Ϳ� ������ǥ���� z-��(0, 0, 1)�� �̷�� ����(����=cos)�� ����Ͽ� �÷��̾��� y-���� ȸ�� ���� m_fYaw�� �����Ѵ�.*/
		m_fYaw = (float)D3DXToDegree(acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &m_d3dxvLook)));
		if (m_d3dxvLook.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		/*���ο� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���� ī�޶� ��尡 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ���� ���� ���� ī�޶��� ���� ��� ���� �����.*/
		m_d3dxvRight = m_pCamera->GetRightVector();
		m_d3dxvUp = m_pCamera->GetUpVector();
		m_d3dxvLook = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		//���� ī�޶� ������ ���ο� ī�޶� ���� ���̴� ������ �����Ѵ�.
		if (!m_pCamera) pNewCamera->CreateShaderVariables(pd3dDevice);
		pNewCamera->SetMode(nNewCameraMode);
		//���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�.
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

//072
void CPlayer::OnPlayerUpdated(float fTimeElapsed)
{
}

//072
void CPlayer::OnCameraUpdated(float fTimeElapsed)
{
}

//072
void CPlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
}

//072
void CPlayer::OnPrepareRender()
{
	m_d3dxmtxWorld._11 = m_d3dxvRight.x;
	m_d3dxmtxWorld._12 = m_d3dxvRight.y;
	m_d3dxmtxWorld._13 = m_d3dxvRight.z;
	m_d3dxmtxWorld._21 = m_d3dxvUp.x;
	m_d3dxmtxWorld._22 = m_d3dxvUp.y;
	m_d3dxmtxWorld._23 = m_d3dxvUp.z;
	m_d3dxmtxWorld._31 = m_d3dxvLook.x;
	m_d3dxmtxWorld._32 = m_d3dxvLook.y;
	m_d3dxmtxWorld._33 = m_d3dxvLook.z;
	m_d3dxmtxWorld._41 = m_d3dxvPosition.x;
	m_d3dxmtxWorld._42 = m_d3dxvPosition.y;
	m_d3dxmtxWorld._43 = m_d3dxvPosition.z;
}

//0720
void CPlayer::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

//08
CAirplanePlayer::CAirplanePlayer(int nMeshes) : CPlayer(nMeshes)
{
}

//072
CAirplanePlayer::~CAirplanePlayer()
{
}

//072
void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	//����� ���� �׸��� ���� x-������ 90�� ȸ���Ѵ�.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, 0.0f, (float)D3DXToRadian(90.0f), 0.0f);
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}
/*3��Ī ī�޶��� �� �÷��̾� �޽��� ���� x-���� �߽����� +90�� ȸ���ϰ� �������Ѵ�. �ֳ��ϸ� ����� �� �޽��� <�׸� 18>�� ���� y-�� ������ ������� ������ �ǵ��� �𵨸��� �Ǿ��� �� �޽��� ī�޶��� z-�� �������� ���ϵ��� �׸� ���̱� �����̴�.*/

//072
//ī�޶� ������ �� ȣ��Ǵ� �Լ��̴�. nNewCameraMode�� ���� ������ ī�޶� ����̴�. 
void CAirplanePlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		//�÷��̾��� Ư���� 1��Ī ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
		SetFriction(200.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		//�÷��̾��� Ư���� �����̽�-�� ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�.
		SetFriction(125.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	case THIRD_PERSON_CAMERA:
		//�÷��̾��� Ư���� 3��Ī ī�޶� ��忡 �°� �����Ѵ�. ���� ȿ���� ī�޶� �������� �����Ѵ�.
		SetFriction(250.0f);
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	//ī�޶� ������ �ð��� ���� �����Ѵ�.
	Update(fTimeElapsed);
}

//08
CTerrainPlayer::CTerrainPlayer(int nMeshes) : CPlayer(nMeshes)
{
}

//08
void CTerrainPlayer::ChangeCamera(ID3D11Device *pd3dDevice, DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		//1��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(D3DXVECTOR3(0.0f, -300.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		//�����̽� �� ī�޶��� �� �÷��̾ �߷��� �ۿ����� �ʴ´�.
		SetGravity(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		//3��Ī ī�޶��� �� �÷��̾ y-�� �������� �߷��� �ۿ��Ѵ�.
		SetGravity(D3DXVECTOR3(0.0f, -300.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(D3DXVECTOR3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);
}

//08
void CTerrainPlayer::OnPlayerUpdated(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
	D3DXVECTOR3 d3dxvScale = pTerrain->GetScale();
	D3DXVECTOR3 d3dxvPlayerPosition = GetPosition();
	int z = (int)(d3dxvPlayerPosition.z / d3dxvScale.z);
	bool bReverseQuad = ((z % 2) != 0);
	/*���� �ʿ��� �÷��̾��� ���� ��ġ (x, z)�� y ���� ���Ѵ�. �׸��� �÷��̾� �޽��� ���̰� 12�̰�
	�÷��̾��� �߽��� ������ü�� ����̹Ƿ� y ���� �޽��� ������ ������ ���ϸ� �÷��̾��� ��ġ�� �ȴ�.*/
	float fHeight = pTerrain->GetHeight(d3dxvPlayerPosition.x, d3dxvPlayerPosition.z, bReverseQuad) + 6.0f;
	/*�÷��̾��� �ӵ� ������ y-���� �����̸�(���� ���, �߷��� ����Ǵ� ���) 
	�÷��̾��� ��ġ ������ y-���� ���� �۾����� �ȴ�. 
	�̶� �÷��̾��� ���� ��ġ�� y ���� ������ ����(������ ������ ���� + 6)���� ������ 
	�÷��̾ ���ӿ� �ְ� �ǹǷ� �÷��̾��� �ӵ� ������ y ���� 0���� ����� 
	�÷��̾��� ��ġ ������ y-���� ������ ���̷� �����Ѵ�. �׷��� �÷��̾�� ���� ���� �ְ� �ȴ�.*/
	if (d3dxvPlayerPosition.y < fHeight)
	{
		D3DXVECTOR3 d3dxvPlayerVelocity = GetVelocity();
		d3dxvPlayerVelocity.y = 0.0f;
		SetVelocity(d3dxvPlayerVelocity);
		d3dxvPlayerPosition.y = fHeight;
		SetPosition(d3dxvPlayerPosition);
	}
}

//08
void CTerrainPlayer::OnCameraUpdated(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	D3DXVECTOR3 d3dxvScale = pTerrain->GetScale();
	CCamera *pCamera = GetCamera();
	D3DXVECTOR3 d3dxvCameraPosition = pCamera->GetPosition();
	int z = (int)(d3dxvCameraPosition.z / d3dxvScale.z);
	bool bReverseQuad = ((z % 2) != 0);
	/*���� �ʿ��� ī�޶��� ���� ��ġ (x, z)�� ����(y ��)�� ���Ѵ�
	. �� ���� ī�޶��� ��ġ�� �ش��ϴ� ������ ���� ���� ������ ī�޶� ���ӿ� �ְ� �ȴ�.
	�̷��� �Ǹ� <�׸� 4>�� ���ʰ� ���� ������ �׷����� �ʴ� ��찡 �߻��Ѵ�(ī�޶� ���� �ȿ� �����Ƿ� ���ε� ������ �ٲ��). 
	�̷��� ��찡 �߻����� �ʵ��� ī�޶��� ��ġ�� �ּҰ��� (������ ���� + 5)�� �����Ѵ�.
	ī�޶��� ��ġ�� �ּҰ��� ������ ��� ��ġ���� ī�޶� ���� �Ʒ��� ��ġ���� �ʵ��� �����Ѵ�.*/
	float fHeight = pTerrain->GetHeight(d3dxvCameraPosition.x, d3dxvCameraPosition.z, bReverseQuad) + 5.0f;
	if (d3dxvCameraPosition.y < fHeight)
	{
		d3dxvCameraPosition.y = fHeight;
		pCamera->SetPosition(d3dxvCameraPosition);
		//3��Ī ī�޶��� ��� ī�޶��� y-��ġ�� ����Ǿ����Ƿ� ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�.
		if (pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}

