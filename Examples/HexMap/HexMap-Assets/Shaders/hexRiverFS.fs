#version 450 core

in VS_OUT
{
	vec2 texCoord;
} fs_in;

out vec4  color;
uniform float time;
uniform sampler2D u_noiseTexture;
float River(vec2 riverUV, sampler2D noiseTexture)
{
	vec2 uv = riverUV;
	uv.x = uv.x * 0.0625f + time * 0.005;
	uv.y -= time*0.25f;
	vec4 noise1 = texture(noiseTexture,uv);

	vec2 uv2 = riverUV;
	uv2.x = uv2.x * 0.0625 - time * 0.0052;
	uv2.y -= time * 0.23;
	vec4 noise2 = texture(noiseTexture, uv2);
	return noise1.x*noise2.w;
}
void main()
{
	//vec2 uv = fs_in.texCoord;
	//uv.x = uv.x * 0.0625f + time * 0.005;
	//uv.y -= time*0.25f;
	//vec4 noise = texture(u_noiseTexture,uv);
	//
	//vec2 uv2 = fs_in.texCoord;
	//uv2.x = uv2.x * 0.0625 - time * 0.0052;
	//uv2.y -= time * 0.23;
	//vec4 noise2 = texture(u_noiseTexture, uv2);
	//vec4 offset = vec4(noise2.a * noise.x, noise2.a * noise.x, noise2.a * noise.x, noise2.a * noise.x);
	color = clamp(vec4(0.0f,0.25f,1.0f,0.2f) + River(fs_in.texCoord, u_noiseTexture), 0.0f,1.0f);
}
