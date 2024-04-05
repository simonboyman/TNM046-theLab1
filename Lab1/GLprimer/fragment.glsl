#version 330 core

out vec4 finalcolor;
in vec3 interpolatedColor;

void main() {
	//finalcolor = vec4(0.5, 0.5, 0.0, 1.0);
	finalcolor = vec4(interpolatedColor, 1.0);
}