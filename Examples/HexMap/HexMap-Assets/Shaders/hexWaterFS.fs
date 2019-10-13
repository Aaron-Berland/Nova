#version 450 core


in VS_OUT
{
	vec3 worldPosition;
} fs_in;

out vec4  color;
uniform float time;
uniform sampler2D u_noiseTexture;

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
	
	//vec2 uv = fs_in.worldPosition.xy*0.025f;
	//uv.y += time*0.055f;
	//vec4 noise = texture(u_noiseTexture,uv);
	//
	//vec2 uv2 = fs_in.worldPosition.xy*0.025f;
	//uv2.x += time*0.055f;
	//vec4 noise2 = texture(u_noiseTexture,uv2);
	//float blendWave = sin((fs_in.worldPosition.x + fs_in.worldPosition.y)*0.085f + (noise.y + noise2.z)+ time*0.2f);
	//blendWave *= blendWave;
	//float waves = mix(noise.z, noise.w,blendWave) + mix(noise2.x, noise2.y,blendWave);
	//waves = smoothstep(0.75f,2.1f,waves);
	//waves *= waves;
	
	//
	//vec2 uv2 = fs_in.texCoord;
	//uv2.x = uv2.x * 0.0625 - time * 0.0052;
	//uv2.y -= time * 0.23;
	//vec4 noise2 = texture(u_noiseTexture, uv2);
	//vec4 offset = vec4(noise2.a * noise.x, noise2.a * noise.x, noise2.a * noise.x, noise2.a * noise.x);
	
	color = clamp(vec4(0.0f,0.25f,1.0f,0.2f) + Waves(fs_in.worldPosition.xy,u_noiseTexture), 0.0f,1.0f);
	//color = offset;
}
