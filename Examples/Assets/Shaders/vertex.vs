#version 450 core

layout (location = 1) in vec3 a_Position;
layout (location = 0) in vec4 a_Color;

out vec3 v_Position;
out vec4 v_Color;
uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_transform;
void main()
{
	v_Color = a_Color;
	v_Position = a_Position;
	gl_Position = u_viewProjectionMatrix * u_transform *vec4(a_Position, 1.0f);
}