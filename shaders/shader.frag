#version 330 core
out vec4 FragColor;

uniform bool uRenderingPoints;

void main() {
    if (uRenderingPoints){
        vec2 coord = gl_PointCoord * 2.0 - 1.0;
        float distSquared = dot(coord,coord);

        if (distSquared > 1.0){
            discard;
        }

        FragColor = vec4(0.1, 0.1, 0.1, 0.4); // cor dos pontos
    }

    else {
        FragColor = vec4(0.2, 0.2, 0.2, 0.4); // cor dos triângulos
    }

}