//#include"shading_functions.hlsli"

//	�~����
static const float PI = 3.141592654f;

//	�K���}�W��
static const float GammaFactor = 2.2f;

//--------------------------------------------
//	�t���l����
//--------------------------------------------
//F0	: �������ˎ��̔��˗�
//VdotH	: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
	return F0 + (1.0f - F0) * pow(clamp(1.0 - VdotH, 0.0f, 1.0f), 5.0f);
}

//--------------------------------------------
//	�g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//diffuse_reflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3  DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuse_reflectance)
{
	return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuse_reflectance / PI);
}

//--------------------------------------------
//	�@�����z�֐�
//--------------------------------------------
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
//roughness : �e��
float  CalcNormalDistributionFunction(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
	return a / (PI * b * b);
}

//--------------------------------------------
//	�􉽌������̎Z�o
//--------------------------------------------
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
	float r = roughness * 0.5f;
	float shadowing = NdotL / (NdotL * (1.0 - r) + r);
	float masking = NdotV / (NdotV * (1.0 - r) + r);
	return shadowing * masking;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmithGGX(float NdotV, float NdotL, float roughness)
{
	float gv = GeometrySchlickGGX(NdotV, roughness);
	float gl = GeometrySchlickGGX(NdotL, roughness);
	return gv * gl;
}

//	���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D��(�@�����z)
	float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	G��(�􉽌�����)
	float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F��(�t���l������)
	float3 F = CalcFresnel(fresnelF0, VdotH);

	return D * G * F / (NdotL * NdotV * 4.0);
}

//walter���f���̔���
float3 SpecularWalterBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 F0, float roughness)
{
	float D = CalcNormalDistributionFunction(NdotH, roughness);
	float G = GeometrySmithGGX(NdotV, NdotL, roughness);
	float3 F = CalcFresnel(F0, VdotH);

	return (D * G * F) / (4.0 * NdotL * NdotV);
}

//Burley�̊g�U���f��
float3 BurleyDiffuseBRDF(float NdotV, float NdotL, float VdotH, float3 diffuse_reflectance, float roughness)
{
	float Fd90 = 0.5 + 2.0 * roughness * VdotH * VdotH;
	float lightScatter = 1.0 + (Fd90 - 1.0) * pow(1.0 - NdotL, 5.0);
	float viewScatter = 1.0 + (Fd90 - 1.0) * pow(1.0 - NdotV, 5.0);

	return diffuse_reflectance * (lightScatter * viewScatter) / PI;
}

//Oren-Nayar �g�UBRDF
float3 OrenNayarDiffuse(float3 N, float3 V, float3 L, float3 diffuseColor, float roughness)
{
	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));

	float3 H = normalize(V + L);
	float VdotH = saturate(dot(V, H));

	float sigma = roughness * roughness;
	float sigma2 = sigma * sigma;

	float A = 1.0 - (0.5 * sigma2 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	float3 Lperp = normalize(L - N * NdotL);
	float3 Vperp = normalize(V - N * NdotV);

	float cos_phi_diff = saturate(dot(Lperp, Vperp)); // �ӂ̍��̗]��

	float alpha = max(NdotL, NdotV);
	float beta = min(NdotL, NdotV);

	return diffuseColor * (A + B * cos_phi_diff * beta) / PI;
}

//--------------------------------------------
//	���b�N�A�b�v�e�[�u������GGX�����擾
//--------------------------------------------
//brdf_sample_point	: �T���v�����O�|�C���g
//lut_ggx_map       : GGX���b�N�A�b�v�e�[�u��
//state             : �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleLutGGX(float2 brdf_sample_point, Texture2D lut_ggx_map, SamplerState state)
{
	return lut_ggx_map.Sample(state, brdf_sample_point);
}

//--------------------------------------------
//	�L���[�u�}�b�v����Ɠx���擾
//--------------------------------------------
//v                     : �T���v�����O����
//diffuse_iem_cube_map  : ���O�v�Z�g�U����IBL�L���[�u�}�b�v
//state                 : �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleDiffuseIEM(float3 v, TextureCube diffuse_iem_cube_map, SamplerState state)
{
	return diffuse_iem_cube_map.Sample(state, v);
}

//--------------------------------------------
//	�L���[�u�}�b�v������ˋP�x���擾
//--------------------------------------------
//v	                        : �T���v�����O����
//roughness                 : �e��
//specular_pmrem_cube_map   : ���O�v�Z���ʔ���IBL�L���[�u�}�b�v
//state                     : �Q�Ǝ��̃T���v���[�X�e�[�g
float4 SampleSpecularPMREM(float3 v, float roughness, TextureCube specular_pmrem_cube_map, SamplerState state)
{
	//�~�b�v�}�b�v�ɂ���đe����\�����邽�߁A�i�K���Z�o
	uint width, height, mip_maps;
	specular_pmrem_cube_map.GetDimensions(0, width, height, mip_maps);
	float lod = roughness * float(mip_maps - 1);
	return specular_pmrem_cube_map.SampleLevel(state, v, lod);
}

//--------------------------------------------
//	�e�����l�������t���l�����̋ߎ���
//--------------------------------------------
//F0	    : �������ˎ��̔��˗�
//VdotN 	: �����x�N�g���Ɩ@���x�N�g���Ƃ̓���
//roughness	: �e��
float3 CalcFresnelRoughness(float3 f0, float NdotV, float roughness)
{
	return f0 + (max((float3)(1.0f - roughness), f0) - f0) * (saturate(1.0f - NdotV), 5.0f);
}

//--------------------------------------------
//	�g�U����IBL
//--------------------------------------------
//normal                : �@��(���K���ς�)
//eye_vector		    : �����x�N�g��(���K���ς�)
//roughness				: �e��
//diffuse_reflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//F0					: �������ˎ��̃t���l�����ːF
//diffuse_iem_cube_map  : ���O�v�Z�g�U����IBL�L���[�u�}�b�v
//state                 : �Q�Ǝ��̃T���v���[�X�e�[�g
float3 DiffuseIBL(float3 normal, float3 eve_vector, float roughness, float3 diffuse_reflectance, float3 f0, TextureCube diffuse_iem_cube_map, SamplerState state)
{
	float3 N = normal;
	float3 V = -eve_vector;

	//�Ԑڊg�U���ˌ��̔��˗�
	float NdotV = max(0.0001f, dot(N, V));
	float3 kD = 1.0f - CalcFresnelRoughness(f0, NdotV, roughness);

	float3 irradiance = SampleDiffuseIEM(normal, diffuse_iem_cube_map, state).rgb;
	return diffuse_reflectance * irradiance * kD;
}

//�Ȉ՗p��DiffuseIBL,�e�N�X�`���̕ς��Ƀ��C�g�J���[�őΉ�
float3 SimpleDiffuseIBL(float3 normal,float3 eve_vector,float roughness,float3 diffuse_reflectance, float3 f0,float3 LC)
{
	float3 N = normal;
	float3 V = -eve_vector;

	//�Ԑڊg�U���ˌ��̔��˗�
	float NdotV = max(0.0001f, dot(N, V));
	float3 kD = 1.0f - CalcFresnelRoughness(f0, NdotV, roughness);
	return diffuse_reflectance * LC * kD;
}

//--------------------------------------------
//	���ʔ���IBL
//--------------------------------------------
//normal				    : �@���x�N�g��(���K���ς�)
//eye_vector			    : �����x�N�g��(���K���ς�)
//roughness				    : �e��
//F0					    : �������ˎ��̃t���l�����ːF
//lut_ggx_map               : GGX���b�N�A�b�v�e�[�u��
//specular_pmrem_cube_map   : ���O�v�Z���ʔ���IBL�L���[�u�}�b�v
//state                     : �Q�Ǝ��̃T���v���[�X�e�[�g
float3 SpecularIBL(float3 normal, float3 eve_vector, float roughness, float3 f0, Texture2D lut_ggx_map, TextureCube specular_pmrem_cube_map, SamplerState state)
{
	float3 N = normal;
	float3 V = -eve_vector;

	float NdotV = max(0.0001f, dot(N, V));
	float3 R = normalize(reflect(-V, N));
	float3 specular_light = SampleSpecularPMREM(R, roughness, specular_pmrem_cube_map, state).rgb;

	float2 brdf_sample_point = saturate(float2(NdotV, roughness));
	float2 env_brdf = SampleLutGGX(brdf_sample_point, lut_ggx_map, state).rgb;
	return specular_light * (f0 * env_brdf.x + env_brdf.y);
}

//float3 SimpleSpecularIBL(float3 normal, float3 eve_vector, float roughness, float3 f0)
//{
//	float3 N = normal;
//	float3 V = -eve_vector;
//
//	float NdotV = max(0.0001f, dot(N, V));
//	float3 R = normalize(reflect(-V, N));
//
//
//}

//--------------------------------------------
//	���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
//diffuse_reflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//F0					: �������ˎ��̃t���l�����ːF
//normal				: �@���x�N�g��(���K���ς�)
//eye_vector			: ���_�Ɍ������x�N�g��(���K���ς�)
//light_vector			: �����Ɍ������x�N�g��(���K���ς�)
//light_color			: ���C�g�J���[
//roughness				: �e��
void DirectBRDF(float3 diffuse_reflectance,
	float3 F0,
	float3 normal,
	float3 eye_vector,
	float3 light_vector,
	float3 light_color,
	float roughness,
	out float3 out_diffuse,
	out float3 out_specular)
{
	float3 N = normal;
	float3 L = -light_vector;
	float3 V = -eye_vector;
	float3 H = normalize(L + V);

	float NdotV = max(0.0001f, dot(N, V));
	float NdotL = max(0.0001f, dot(N, L));
	float NdotH = max(0.0001f, dot(N, H));
	float VdotH = max(0.0001f, dot(N, H));

	float3 irradiance = light_color * NdotL;

	//�g�U����BRDF
	out_diffuse = DiffuseBRDF(VdotH, F0, diffuse_reflectance) * irradiance;
	//out_diffuse = BurleyDiffuseBRDF(NdotV, NdotL, VdotH, diffuse_reflectance, roughness) * irradiance;
	//out_diffuse = OrenNayarDiffuse(N, V, L, diffuse_reflectance, roughness) * irradiance;

	//	���ʔ���BRDF
	out_specular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
	//out_specular = SpecularWalterBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}