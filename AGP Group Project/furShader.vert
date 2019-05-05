// furShader.vert
// use textures, but no lighting or materials
#version 330

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

layout (location = 0) in  vec3 in_Position;
layout (location = 1) in  vec3 in_Normal;
layout (location = 2) in  vec2 in_TexCoord;

out vec3 out_Position;
out vec3 out_Normal;

out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 viewSpace;

uniform  float furFlowOffset;
uniform  float currentLayer;
uniform  float layers;
uniform	 float furLength;

uniform vec3 LightInvDirection_worldspace;

out vec2 ex_TexCoord;

vec4 vGravity = vec4(0.0f, -2.0f, 0.0f, 1.0f);

void main(void) {

	// Make the modelview
	mat4 modelview = V * M;

	viewSpace = V * M * vec4(in_Position,1);

	// Extrude the surface by the normal by the gap
	vec3 Pos = in_Position.xyz + (in_Normal * (currentLayer * (furLength / layers)));
	// Translate into worldspace
	vec4 Point = (modelview * vec4(Pos,1.0));

	// As the layers gets closer to the tip, bend more
	float layerNormalize = (currentLayer / layers);
	vGravity = (vGravity * modelview);
	float k = pow(layerNormalize, 3) * 0.08;
	Point = Point + vGravity * k;
	if(currentLayer != 0){
		Point = Point + vec4(1.0f, 1.0f, 1.0f, 1.0f) * (furFlowOffset);
	}

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	EyeDirection_cameraspace = vec3(0,0,0) - ( V * M * vec4(in_Position,1)).xyz;

	// Vector that goes from the vertex to the light, in camera space
	LightDirection_cameraspace = (V*vec4(LightInvDirection_worldspace,0)).xyz;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( V * M * vec4(in_Normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	ex_TexCoord = in_TexCoord;
    gl_Position = P * Point;

	out_Position = in_Position;
	out_Normal = in_Normal;
}

