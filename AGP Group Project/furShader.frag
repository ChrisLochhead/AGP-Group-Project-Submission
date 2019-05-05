// furShader.frag
// use textures, but no lighting or materials
#version 330

struct DirLight
{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Some drivers require the following
precision highp float;
in vec2 ex_TexCoord;
uniform float UVScale;
uniform float layers;
uniform float currentLayer;

in vec3 out_Position;
in vec3 out_Normal;

in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

uniform sampler2D textureUnit0;
uniform sampler2D textureUnit1;
uniform DirLight dirLight;

//fog uniforms
uniform int fogSelector;
//0 plane based; 1 range based
uniform int depthFog;

//fog variables
const vec3 fogColor = vec3(0.5, 0.5,0.5);
const float FogDensity = 0.08;

//fog shading
in vec4 viewSpace;

layout(location = 0) out vec4 out_Color;
vec4 furColour;
vec4 baseColour = texture(textureUnit1, ex_TexCoord);

vec3 CalcDirLight( DirLight light, vec3 normal, float cosA, float visibility, float cosT )
{
	 vec3 lightDir = normalize( -light.direction );
    
    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );
    
    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    //float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );
    
    // Combine results
    vec3 diffuse = light.diffuse  * diff * vec3( texture( textureUnit1,  ex_TexCoord ) );
    vec3 ambient =visibility * light.ambient * vec3( texture( textureUnit1, ex_TexCoord ) );
    vec3 specular = visibility * light.specular * cosA* vec3( texture( textureUnit1, ex_TexCoord ) );

    //vec3 MaterialDiffuseColor = texture( textureUnit1, ex_TexCoord ).rgb;
   // vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	//vec3 MaterialSpecularColor = vec3( texture( textureUnit1, ex_TexCoord) );
    
    return ( ambient + diffuse + specular );
}

void main(void) {
	// Make Base of object a texture
	
	if(currentLayer > 0)
	{
	//////////////////////////////////// fur texture modifiying //////////////////////////////////
		furColour = texture(textureUnit0, ex_TexCoord);
		// Tells OpenGL what to do with transparency.
		if(furColour.a < 0.1) discard;

		// Else to replace pixel colours
		if(furColour.r < 0.1) discard;
		else furColour.r = baseColour.r;
		if(furColour.g < 0.1) discard;
		else furColour.g = baseColour.g;
		if(furColour.b < 0.1) discard;
		else furColour.b = baseColour.b;
	}
	vec3 norm = normalize( Normal_cameraspace );
	vec3 n = normalize( Normal_cameraspace );
	vec3 l = normalize( LightDirection_cameraspace );
	float visibility=1.0;

	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n);
	float cosTheta = clamp( dot( n,l ), 0,1 );
	float cosAlpha = clamp( dot( E,R ), 0,0.1 );
	
	furColour = vec4(CalcDirLight(dirLight, norm, cosAlpha, UVScale, cosTheta),1.0);
	/////////////////////////////////fog shading/////////////////////////////////

	vec3 RimColor = vec3(0.2, 0.2, 0.2);
	vec3 tex1 = texture(textureUnit1, ex_TexCoord).rgb;
 
	//get light an view directions
	vec3 L = normalize( LightDirection_cameraspace);
	vec3 V = normalize( EyeDirection_cameraspace);
 
	//diffuse lighting
	vec3 fogNorm = normalize( Normal_cameraspace );
	vec3 diffuse = dirLight.diffuse * max(0, dot(L, fogNorm));
 
	//rim lighting
	float rim = 1 - max(dot(V, fogNorm), 0.0);
	rim = smoothstep(0.6, 1.0, rim);
	vec3 finalRim = RimColor * vec3(rim, rim, rim);
	//get all lights and texture
	vec3 lightColor = finalRim + diffuse + tex1;
 
	vec3 finalColor = vec3(0, 0, 0);
 
	//distance
	float dist = 0;
	float fogFactor = 0;
 
	//compute distance used in fog equations
	if(depthFog == 0)//select plane based vs range based
	{
		//plane based
		dist = abs(viewSpace.z);
		//dist = (gl_FragCoord.z / gl_FragCoord.w);
	}
	else
	{
		//range based
		 dist = length(viewSpace);
	}
 
	if(fogSelector == 0)//linear fog
	{
		// 20 - fog starts; 80 - fog ends
		fogFactor = (80 - dist)/(80 - 20);
		fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
		 //if you inverse color in glsl mix function you have to
		//put 1.0 - fogFactor
		finalColor = mix(fogColor, furColour.rgb, fogFactor);
		}
	else if( fogSelector == 1)// exponential fog
	{
		fogFactor = 1.0 /exp(dist * FogDensity);
		fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
		// mix function fogColor⋅(1−fogFactor) + lightColor⋅fogFactor
		finalColor = mix(fogColor, furColour.rgb, fogFactor);
	}
	else if( fogSelector == 2)
	{
	   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
	   fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
	   finalColor = mix(fogColor, furColour.rgb, fogFactor);
	}

	////////////////////////////////////////////
	furColour = vec4(finalColor, 1.0);

	//////////////////////////////////// output //////////////////////////////////
	// Fragment colour
	furColour.w = UVScale;
	out_Color = furColour;
	if(currentLayer == 0) out_Color = vec4(0.0,0.0,0.0,1.0);
}