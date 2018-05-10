#version 400
precision highp float;
in  vec3 ex_Color;
in	vec2 uv_Color;

out vec4 FragColor;

uniform sampler2D text;

void main(void) {
	vec4 ambient_Color = vec4(0.1f, 0.1f, 0.1f, 1.0f);
    FragColor = ambient_Color + (texture2D(text,uv_Color) * vec4(ex_Color, 1.f));
}
