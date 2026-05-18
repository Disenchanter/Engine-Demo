#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

// aPos is provided as a vertex attribute / 頂点属性として渡される aPos
// Do not modify the input layout / 入力レイアウトは変更しないでください
void main()
{
    // Convert input vertex position to homogeneous coordinates (3D -> 4D)
	vec4 transformPosition = vec4(aPos, 1.0);

	// Multiply by model matrix to get world-space position
	transformPosition = modelMatrix * transformPosition;

	// Pass world position to fragment shader
	worldPosition = transformPosition.xyz;

	gl_Position = projectionMatrix * viewMatrix * transformPosition;
	
	uv = aUV;
	// normal matrix transforms normals to world space
	normal =  normalMatrix * aNormal;
}