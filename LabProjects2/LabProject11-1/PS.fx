//�ȼ� ���̴����� ����� ������۸� ������ �� �� �κп� ������ ���� �߰��Ѵ�.
cbuffer cbColor : register(b0)
{
	float4	gcColor : packoffset(c0);
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

//���̴� ����("Effect.fx")�� ������ ���� �ȼ� - ���̴� �Լ��� �߰��Ѵ�.
// �ȼ�-���̴�
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
	//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
}
//�ȼ����̴�. �̰� ������ �ƿ�ǲ ������ ���ϰ��� ����.
//�ø�ƽ�� ���ΰ� ������, SV_Target�� SV_Target0�� ����.
//���� �ȼ����̴��� Į�� �����ؾ� �Ѵ�.
