#version 450 core

in VS_OUT
{
	vec3 worldPosition;
	vec2 texCoord0;
	vec2 texCoord1;
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
float Foam(float shore, vec2 worldposXY, sampler2D noiseTexture)
{
	shore = sqrt(shore);
	vec2 noiseUV = fs_in.worldPosition.xy*0.025f + time*0.01f;
	vec4 noise = texture(noiseTexture,noiseUV);

	float distortion1 = noise.x * (1 - shore);
	float foam1 = sin((shore+distortion1) * 10 - time);
	foam1 *= foam1;

	float distortion2 = noise.y * (1 - shore);
	float foam2 = sin((shore + distortion2) * 10 + time + 2);
	foam2 *= foam2 * 0.7;

	return max(foam1, foam2) * shore;
}
float Waves(vec2 worldposXY, sampler2D noiseTexture)
{
	vec2 uv = worldposXY*0.025f;
	uv.y += time*0.055f;
	vec4 noise = texture(noiseTexture,uv);

	vec2 uv2 = worldposXY*0.025f;
	uv2.x += time*0.055f;
	vec4 noise2 = texture(noiseTexture,uv2);
	float blendWave = sin((worldposXY.x + worldposXY.y)*0.085f + (noise.y + noise2.z)+ time*0.2f);
	blendWave *= blendWave;
	float waves = mix(noise.z, noise.w,blendWave) + mix(noise2.x, noise2.y,blendWave);
	return smoothstep(0.75f,2.1f,waves);
}
void main()
{
	float shore = fs_in.texCoord0.y;
	float foam = Foam(shore, fs_in.worldPosition.xy,u_noiseTexture);
	float waves = Waves(fs_in.worldPosition.xy,u_noiseTexture);
	waves *= 1 - shore;
	float shoreWater =  max(foam,waves);
	float river = River(fs_in.texCoord1, u_noiseTexture);
	float water = mix(shoreWater, river, fs_in.texCoord0.x);
	//color = clamp(vec4(0.0f,0.25f,1.0f,0.2f) + max(foam,waves), 0.0f,1.0f);
	//color = vec4(fs_in.texCoord1,1.0f,1.0f);
	color = clamp(vec4(0.0f,0.25f,1.0f,0.2f) + water , 0.0f,1.0f);
 }
