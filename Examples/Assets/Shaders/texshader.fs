#version 450 core

layout (location = 0)  out vec4 color;

in vec2 v_TexCoord;
//uniform sampler2D u_Texture;

void main()
{
	//color = texture(u_Texture,v_TexCoord);
	color = vec4(v_TexCoord.x, v_TexCoord.y, 1.0f,1.0f);
}