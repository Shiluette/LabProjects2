#pragma once

//0730
class AABB
{
public:
	//바운딩 박스의 최소점과 최대점을 나타내는 벡터이다.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//두 개의 바운딩 박스를 합한다.
	void Merge(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Merge(AABB *pAABB);
	//바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산한다.
	void Update(D3DXMATRIX *pd3dxmtxTransform);
};

class CVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
public:
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }
};

//052
class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//생성자와 소멸자를 선언한다.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

//075
class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//Mesh.cpp 파일에서 CMesh::AddRef(), CMesh::Release() 함수의 정의를 삭제한다. 

protected:
	AABB m_bcBoundingCube;

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//정점의 위치 벡터와 색상을 저장하기 위한 버퍼에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dPositionBuffer;
	ID3D11Buffer *m_pd3dColorBuffer;

	//버퍼들을 입력조립기에 연결하기 위한 시작 슬롯 번호이다. 
	UINT m_nSlot;
	/*인스턴싱을 위한 정점 버퍼는 메쉬의 정점 데이터와 인스턴싱 데이터(객체의 위치와 방향)를 갖는다. 그러므로 인스턴싱을 위한 정점 버퍼는 하나가 아니라 버퍼들의 배열이다. 정점의 요소들을 나타내는 버퍼들을 입력조립기에 전달하기 위한 버퍼이다.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//정점을 조립하기 위해 필요한 버퍼의 개수이다. 
	int m_nBuffers;

	//정점의 개수이다. 
	int m_nVertices;
	UINT m_nStartVertex;
	//정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열이다. 
	UINT *m_pnVertexStrides;
	//정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다. 
	UINT *m_pnVertexOffsets;

	//인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dIndexBuffer;
	//인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	UINT m_nIndices;
	//인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	UINT m_nStartIndex;
	//각 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//각 인덱스의 형식(DXGI_FORMAT_R32_UINT 또는 DXGI_FORMAT_R16_UINT)이다. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	ID3D11RasterizerState *m_pd3dRasterizerState;

public:
	AABB GetBoundingCube() { return(m_bcBoundingCube); }

	//메쉬의 정점 버퍼들을 배열로 조립한다. 
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//인스턴싱을 사용하여 렌더링한다. 
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);
};

//class CTriangleMesh : public CMesh
//{
//public:
//	CTriangleMesh(ID3D11Device *pd3dDevice);
//	virtual ~CTriangleMesh();
//
//	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
//
//	//05-1
//	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
//};

//06
class CCubeMesh : public CMesh
{
public:
	//072
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//072
class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMesh();
};

//073
class CSphereMesh : public CMesh
{
public:
	CSphereMesh(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CSphereMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};

