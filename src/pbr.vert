#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNor;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPosWorldSpace;
out vec2 TexCoord;
out vec3 Tangent;

void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0);
	Normal = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vec4(vPos, 1.0));
	TexCoord = vTexCoord;
	Tangent = vTangent;
}
