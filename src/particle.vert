#version 450 core

layout(location = 0) in vec2 aPos;	// Quad vertex
layout(location = 1) in vec4 aInstanceData;	//xyz pos, w size
layout(location = 2) in int aTileIndex;
layout(location = 3) in float normalisedLifetime;
layout(location = 4) in int aIsSmoke; // 1 or 0

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
flat out int tileIndex;
out float lifeTime;
flat out int isSmoke;


void main()
{
	vec3 worldPos = aInstanceData.xyz;
	float size = aInstanceData.w;

	// Bilboard particle
	vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

	vec3 offset = (aPos.x * right + aPos.y * up) * size;
	vec3 finalPos = worldPos + offset;

	gl_Position = projection * view * vec4(finalPos, 1.0);

	TexCoord = aPos + vec2(0.5);	// map quad (-0.5..0.5) to (0..1)
	tileIndex = aTileIndex;
	lifeTime = normalisedLifetime;
	isSmoke = aIsSmoke;
}