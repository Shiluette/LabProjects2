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

//075
struct VS_INSTANCED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
	//���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ��ġ �����̴�.
	float3 instancePos : POSINSTANCE;
};

//075
struct VS_INSTANCED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	//�ý��� ���� ������ ���� ���̴��� ���޵Ǵ� ��ü �ν��Ͻ��� ID�� �ȼ� ���̴��� �����Ѵ�.
	float4 instanceID : INDEX;
};

//075
VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
	//������ ��ġ ���Ϳ� �ν��Ͻ��� ��ġ ����(���� ��ǥ)�� ���Ѵ�.
	//output.position = input.position + float4(input.instancePos, 0.0f); //OK
	//output.position = float4(input.position, 1.0f) + input.instancePos; //OK
	//output.position = input.position + float4(input.instancePos, 1.0f); //OK
	//output.position = float4(input.position, 0.0f) + float4(input.instancePos, 1.0f); //OK
	output.position = float4(input.position, 1.0f) + float4(input.instancePos, 0.0f); //OK
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//�ν��Ͻ� ID�� �ȼ� ���̴��� �����Ѵ�.
	output.instanceID = instanceID;
	return output;
}

//075
float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
{
	return input.color;
}