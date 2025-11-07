#include "SkinnedMesh.hlsli"

[maxvertexcount(6)]
void main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> output)
{
	//•\–Ê“o˜^
	int i;
	for (i = 0; i < 3; i++)
	{
		GS_OUT element = (GS_OUT)0;
		element.vs = input[i];
		element.is_outline = false;
		output.Append(element);
	}
	output.RestartStrip();

	//	— –Ê“o˜^
	for (i = 2; i >= 0; i--)
	{
        GS_OUT element = (GS_OUT) 0;
        element.vs = input[i];

        // –@ü•ûŒü‚É‰Ÿ‚µo‚·
        element.vs.world_position.xyz = input[i].world_position.xyz + input[i].world_normal.xyz * lineSize;
        element.vs.world_position.w = 1.0;

        // –@ü‚ğ”½“]i— –Ê—pj
        element.vs.world_normal.xyz = -input[i].world_normal.xyz;

        // MVP•ÏŠ·
        element.vs.position = mul(element.vs.world_position, view_projection);

        element.is_outline = true;
        output.Append(element);
    }
	output.RestartStrip();
}