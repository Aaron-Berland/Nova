#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;


out VS_OUT
{
	vec3 normal;
	vec4 vertexColor;
} vs_out;

uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_transform;
void main()
{
	gl_Position = u_viewProjectionMatrix * u_transform * vec4(a_Position,1.0);

	vs_out.vertexColor = a_Color;
/*
	TODO(BYRON): mvp is not correct
*/
	vs_out.normal = a_Normal; 
}
