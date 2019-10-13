#version 450 core

in VS_OUT
{
	vec3 worldPosition;
	vec2 texCoord;
} fs_in;

out vec4  color;
uniform sampler2D u_noiseTexture;
uniform float time;

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
	vec4 noise = texture(u_noiseTexture,uv);

	vec2 uv2 = worldposXY*0.025f;
	uv2.x += time*0.055f;
	vec4 noise2 = texture(u_noiseTexture,uv2);
	float blendWave = sin((worldposXY.x + worldposXY.y)*0.085f + (noise.y + noise2.z)+ time*0.2f);
	blendWave *= blendWave;
	float waves = mix(noise.z, noise.w,blendWave) + mix(noise2.x, noise2.y,blendWave);
	return smoothstep(0.75f,2.1f,waves);
}
void main()
{
	float shore = fs_in.texCoord.y;
	float foam = Foam(shore, fs_in.worldPosition.xy,u_noiseTexture);
	float waves = Waves(fs_in.worldPosition.xy,u_noiseTexture);
	waves *= 1 - shore;

	color = clamp(vec4(0.0f,0.25f,1.0f,0.2f) + max(foam,waves), 0.0f,1.0f);
	//color = vec4(fs_in.texCoord , 1.0f,1.0f);
}
