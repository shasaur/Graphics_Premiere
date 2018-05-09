#version 400
precision highp float;
in  vec3 ex_Color;
out vec4 FragColor;

void main(void) {
	vec3 ambient_Color = vec3(0.1f, 0.1f, 0.1f);

    FragColor = vec4(ambient_Color+ex_Color,1.0);
}
