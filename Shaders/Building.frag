#version 450

layout (location = 3) uniform vec3 cameraPosition;

layout (location = 0) out vec4 fragment;

uniform sampler2D shadowMap;

in vec3 position;
in vec3 normal;
in vec4 shadowCoord;
in float textureIndex;
in float isConnection;

float computeShadow(vec4 fragPosLightSpace) 
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.001f;
	
	float shadow = 0.0f;
	vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
	for(int x = -3; x <= 3; ++x)
		for(int y = -3; y <= 3; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			float intensity = currentDepth - closestDepth;
			if(intensity < 0.0f)
				intensity = 0.0f; 
			shadow += currentDepth - bias > pcfDepth ? 0.0f : 1.0f;        
		}    
	shadow /= 49.0f;

    return shadow;
}

void main()
{
	vec3 light = vec3(1.0f, 1.0f, 1.0f);
	light = normalize(light);
	float diffuse = dot(normalize(normal), light);
	
	if(diffuse < 0.0f)
		diffuse = 0.0f;
	
	vec3 cameraRay = normalize(cameraPosition - position);
	
	float fresnelAngle = dot(normalize(normal), cameraRay);
	
	if(fresnelAngle < 0.0f)
		fresnelAngle = 0.0f;
		
	fresnelAngle = pow(1.0f - fresnelAngle, 4.0f);
		
	float angle = diffuse * 0.8f + 0.2f;
	
	float shadow = computeShadow(shadowCoord);
	shadow = shadow * 0.8f + 0.2f;
	
	/*if(diffuse > 0.0f)
		angle *= shadow;
	else if(angle * shadow < angle)
		angle *= shadow;*/
		
	if(shadow < angle)
		angle = shadow;
		
	fresnelAngle *= 0.2f;
	
	angle += fresnelAngle;
	
	if(angle > 1.0f)
		angle = 1.0f;
	
	vec3 roofColor = vec3(0.8f, 0.4f, 0.1f);
	vec3 wallColor = vec3(1.0f, 0.98f, 0.92f);
	//vec3 wallColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 color = (textureIndex > 0.5f ? roofColor : wallColor) * angle;
	
	vec3 reflectAngle = reflect(-light, normalize(normal));
	
	float specular = dot(normalize(reflectAngle), cameraRay);
	
	if(specular < 0.0f)
		specular = 0.0f;
	
	specular = pow(specular, 8.0f) * 0.5f + pow(specular, 64.0f) * 0.5f;
	
	specular *= 0.7f;
	
	specular *= shadow;
	
	color = color * (1.0f - specular) + vec3(specular);

	if(isConnection > 0.5f)
	{
		color = color * 0.5f + vec3(1.0f, 0.0f, 0.0f) * 0.5f;
	}

	fragment = vec4(color.rgb, 1.0f);
}
