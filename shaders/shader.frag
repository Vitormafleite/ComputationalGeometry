#version 330 core
out vec4 FragColor;

uniform bool uRenderingPoints;
uniform bool uWireframePass;
uniform bool uRenderingGrid;

void main() {
    if (uRenderingPoints){
        vec2 coord = gl_PointCoord * 2.0 - 1.0;
        float distSquared = dot(coord,coord);

        if (distSquared > 1.0){
            discard;
        }

        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // cor dos pontos
    }
    else if (uRenderingGrid) {
        FragColor = vec4(0.2, 0.2, 0.2, 0.8); // Grid color: darker gray with higher opacity
    }
    else if (uWireframePass) {
        FragColor = vec4(0.14, 0.14, 0.14, 0.9); // black wireframe overlay
    }
    else {
        if (gl_FrontFacing)
            FragColor = vec4(0.4, 0.4, 0.4, 1.0); // cor dos triângulos
        else
            FragColor = vec4(0.55, 0.55, 0.55, 1.0); // Back face: Blue
    }
}