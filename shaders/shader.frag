#version 330 core
out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord * 2.0 - 1.0; // convert [0,1] → [-1,1]
        if (dot(coord, coord) > 1.0) {
            discard; // Outside unit circle → skip pixel
        }

    FragColor = vec4(0.2, 0.6, 0.8, 1.0);
}
