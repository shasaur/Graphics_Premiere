#version 400

precision highp float;

in vec3 in_Position;
in vec3 in_Color;


uniform mat4 mvpmatrix;  // mvpmatrix is the result of multiplying the model, view, and projection matrices

out vec3 ex_Color;
void main(void) {

    gl_Position = mvpmatrix * vec4(in_Position, 1.0f); // Multiply the mvp matrix by the vertex to obtain our final vertex position
    
    ex_Color = in_Color;
}