#version 450 core

layout (location = 0) out vec4 fColour;

in vec2 TexCoord;
flat in int tileIndex;
in float lifeTime;
flat in int isSmoke;

uniform sampler2D particleTexture;
uniform sampler2D smokeTexture;

const int frameX = 3; // cols
const int frameY = 2; // rows

vec2 getSpriteUV(vec2 baseUV, int index) 
{
	int x = index % frameX;
	int y = index / frameX;

	vec2 cellSize = vec2(1.0 / float(frameX), 1.0 / float(frameY));
	vec2 offset = vec2(x, y) * cellSize;

	return baseUV * cellSize + offset;
}

vec4 getLifetimeColourFlame(float t)
{
	// Orange to dark grey
	vec3 startColour = vec3(1.0, 0.5, 0.1);
	vec3 endColour = vec3(0.2, 0.2, 0.2);
	float alpha = smoothstep(1.0, 0.0, t);

	float blend = pow(t, 0.5);

	return vec4(mix(startColour, endColour, 1.0 - blend), alpha);
}

vec4 getLifetimeColourSmoke(float t)
{
	// white to dark grey
	vec3 startColour = vec3(1, 1, 1);
	vec3 endColour = vec3(0.5, 0.5, 0.5);
	float alpha = smoothstep(1.0, 0.0, t);

	return vec4(mix(endColour, startColour, t), alpha);
}

void main()
{
	vec2 uv = getSpriteUV(TexCoord, tileIndex);
	vec4 texColour;
	vec4 colour;
	if (isSmoke == 1) {
		texColour = texture(smokeTexture, uv);
		colour = getLifetimeColourSmoke(lifeTime) * texColour;
	} else {
		texColour = texture(particleTexture, uv);
		colour = getLifetimeColourFlame(lifeTime) * texColour;
	}
		
	if (texColour.a < 0.1)
		discard;
	fColour = colour;
}