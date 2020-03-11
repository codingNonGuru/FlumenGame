#version 450

layout (location = 3) uniform vec3 cameraPosition;

layout (location = 0) out vec4 fragment;

in vec3 position;
in vec3 normal;

void main()
{
	vec3 light = vec3(1.0f, 1.0f, 1.0f);
	light = normalize(light);
	float angle = dot(normalize(normal), light);
	
	if(angle < 0.0f)
		angle = 0.0f;
	
	vec3 cameraRay = normalize(cameraPosition - position);
	
	float fresnelAngle = dot(normalize(normal), cameraRay);
	
	if(fresnelAngle < 0.0f)
		fresnelAngle = 0.0f;
		
	fresnelAngle = pow(1.0f - fresnelAngle, 16.0f);
	
	//fresnelAngle *= 0.5f;
	
	angle += fresnelAngle;
	
	if(angle > 1.0f)
		angle = 1.0f;
		
	angle = angle * 0.8f + 0.2f;
	
	vec3 color = vec3(1.0f, 0.0f, 0.0f) * angle;
	
	vec3 reflectAngle = reflect(-light, normalize(normal));
	
	float specular = dot(normalize(reflectAngle), cameraRay);
	
	if(specular < 0.0f)
		specular = 0.0f;
	
	specular = pow(specular, 8.0f) * 0.5f + pow(specular, 64.0f) * 0.5f;
	
	//specular *= 0.5f;
	
	color = color * (1.0f - specular) + vec3(specular);

	fragment = vec4(color.rgb, 1.0f);
}
