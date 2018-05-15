#version 400

precision highp float;

in vec3 in_Position;
in vec3 in_Color;
in vec3 in_Normal;
in vec3 in_Angle;
in vec2 in_UV;

uniform mat4 mvpmatrix;  // mvpmatrix is the result of multiplying the model, view, and projection matrices
uniform mat4 modelmatrix;
uniform mat4 viewmatrix;
uniform mat4 anglematrix;
uniform vec3 camerapos;

out vec3 ex_Color;
out vec2 uv_Color;

void main(void) {
    gl_Position = mvpmatrix * vec4(in_Position, 1.0f); // Multiply the mvp matrix by the vertex to obtain our final vertex position

	vec3 lightPos = vec3(0.0,0.0,100.0);
	vec3 lightColour = vec3(1.0,1.0,1.0);

	vec3 normal = vec3(normalize(transpose(inverse(modelmatrix)) * vec4(in_Normal, 1.0f)));
	vec3 surfacePos = vec3(modelmatrix * vec4(in_Position, 1.0));
	vec4 surfaceColor = vec4(1.0,1.0,1.0, 1.0);
	vec3 surfaceToLight = normalize(lightPos - surfacePos);
	vec3 surfaceToCamera = normalize(camerapos - surfacePos); //also a unit vector

	// Diffuse
	float diffuseCoefficient = max(0.0, dot(normalize((anglematrix*vec4(in_Normal,1.0)).xyz), normalize(lightPos)));
	vec3 diffuse = diffuseCoefficient * lightColour;
	//vec3 diffuseReflection2 = vec3(1.0,1.0,1.0) * max(0.0, dot((anglematrix*vec4(in_Normal, 1.0f)), vec4(lightPos, 1.0)));

	// Specular
	//vec3 normal = normalize(transpose(inverse(mat3(angle))) * in_Normal);
	float materialShininess = 100.0;
	float specularCoefficient = 0.0;
	if(diffuseCoefficient > 0.0)
		specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess);
	vec3 specular = specularCoefficient * vec3(1.0,1.0,1.0);

    ex_Color = in_Color * (diffuse+specular);// + specular);
	uv_Color = in_UV;
}
