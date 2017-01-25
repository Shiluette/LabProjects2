//ī�޶� ��ȯ ��İ� ���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 0�� ���).
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ��ȯ ����� ���� ���̴� ������ �����Ѵ�(���� 1�� ���). 
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
// ����-���̴� 
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
	return output;
}

//052
// �ȼ�-���̴�
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
}

////075
//struct VS_INSTANCED_COLOR_INPUT
//{
//	float3 position : POSITION;
//	float4 color : COLOR0;
//	//���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ��ġ �����̴�.
//	float3 instancePos : POSINSTANCE;
//};

//0752
struct VS_INSTANCED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	column_major float4x4 mtxTransform : INSTANCEPOS;
	float4 instanceColor : INSTANCECOLOR;
};

//075
struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//�ý��� ���� ������ ���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ID�� �ȼ� ���̴��� �����Ѵ�.
	float4 instanceID : INDEX;
};

//0752
VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	output.position = mul(float4(input.position, 1), input.mtxTransform);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = (instanceID % 5) ? input.color : (input.color * 0.3f + input.instanceColor * 0.7f);
	output.instanceID = instanceID;
	return output;
}

//075
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	return input.color;
}