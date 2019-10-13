#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord0;
layout (location = 2) in vec2 a_TexCoord1;
out VS_OUT
{
	vec3 worldPosition; 
	vec2 texCoord0;
	vec2 texCoord1;
} vs_out;


uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_transform;
void main()
{
	gl_Position = u_viewProjectionMatrix * u_transform * vec4(a_Position,1.0);
	vs_out.texCoord0 = a_TexCoord0;
	vs_out.texCoord1 = a_TexCoord1;
	vs_out.worldPosition = a_Position;
}
