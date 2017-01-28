//픽셀 쉐이더에서 사용할 상수버퍼를 파일의 맨 앞 부분에 다음과 같이 추가한다.
cbuffer cbColor : register(b0)
{
	float4	gcColor : packoffset(c0);
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

//쉐이더 파일("Effect.fx")에 다음과 같이 픽셀 - 쉐이더 함수를 추가한다.
// 픽셀-쉐이더
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
	//입력되는 정점의 색상을 그대로 출력한다. 
}
//픽셀쉐이더. 이거 다음엔 아웃풋 머져가 리턴값을 받음.
//시맨틱이 제로가 없으면, SV_Target은 SV_Target0와 동일.
//보통 픽셀쉐이더는 칼라를 리턴해야 한다.

