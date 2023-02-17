#version 330 core
out vec4 FragColor;

in vec3 TexCoord;
in vec4 pos;

// texture samplers
uniform sampler3D densityTexture;
uniform vec3 camPos;
uniform vec3 bgColor;
uniform vec3 smokeColor;

void main() {
    // Refering from Real Time Rendering 4th edition 14.2.
    vec3 currentPos = pos.xyz;
    vec3 direction = currentPos - camPos;
    direction = normalize(direction);
    float step = 0.01;
    vec4 density = vec4(0.0f);
    vec3 currentColor = bgColor;

    while(true) {
        if (currentPos.x > 0.5 || currentPos.x < -0.5
         || currentPos.y > 0.5 || currentPos.y < -0.5
         || currentPos.z > 0.5 || currentPos.z < -0.5) {
            break;
        }
        vec3 currentTexCoord = currentPos + 0.5;

        density = texture(densityTexture, currentTexCoord);
        float f = exp(-density.x * 2. * step);
        currentColor = f * currentColor + (1-f) * smokeColor;
        currentPos += step * direction;
    }
    FragColor = vec4(currentColor, 1.0f);
}