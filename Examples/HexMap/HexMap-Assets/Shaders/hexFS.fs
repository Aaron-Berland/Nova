#version 450 core
in VS_OUT
{
	vec3 normal;
	vec4 vertexColor;

} fs_in;
out vec4 color;

void main()
{
	vec3 n = normalize(fs_in.normal);
	vec3 lightDir = normalize(vec3(0.2f,0.2f,1.0f));
	color = vec4((0.3f+0.7f*max(0.0f,dot(n,lightDir)))*fs_in.vertexColor.xyz,fs_in.vertexColor.w);
	//color = vec4(1.0f,1.0f,1.0f,1.0f);
}