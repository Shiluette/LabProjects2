//카메라 변환 행렬과 투영 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 0을 사용).
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//월드 변환 행렬을 위한 쉐이더 변수를 선언한다(슬롯 1을 사용). 
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

//0510
cbuffer cbColor : register(b0)
{
	float4	gcColor : packoffset(c0);
};

//052
struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

//052
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

//052
// 정점-쉐이더 
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//입력되는 정점의 색상을 그대로 출력한다. 
	return output;
}

//052
// 픽셀-쉐이더
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//입력되는 정점의 색상을 그대로 출력한다. 
}

//075
struct VS_INSTANCED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	//정점 쉐이더에 전달되는 객체 인스턴스의 위치 벡터이다.
	float3 instancePos : POSINSTANCE;
};

//075
struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//시스템 생성 변수로 정점 쉐이더에 전달되는 객체 인스턴스의 ID를 픽셀 쉐이더로 전달한다.
	float4 instanceID : INDEX;
};

//075
VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	//정점의 위치 벡터에 인스턴스의 위치 벡터(월드 좌표)를 더한다.
	//output.position = input.position + float4(input.instancePos, 0.0f); //OK
	//output.position = float4(input.position, 1.0f) + input.instancePos; //OK
	//output.position = input.position + float4(input.instancePos, 1.0f); //OK
	//output.position = float4(input.position, 0.0f) + float4(input.instancePos, 1.0f); //OK
	output.position = float4(input.position, 1.0f) + float4(input.instancePos, 0.0f); //OK
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//인스턴스 ID를 픽셀 쉐이더로 전달한다.
	output.instanceID = instanceID;
	return output;
}

//075
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	return input.color;
}