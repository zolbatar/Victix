#pragma once

const char *crt_vertex_shader = R"(
#version 150

in vec2 position;
in vec2 texCoords;
out vec2 TexCoords;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    TexCoords = texCoords;
}
)";

const char *crt_fragment_shader = R"(
#version 150

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;

const float PI = 3.1415926535897932384626433832795;

// Parameters for the CRT effect
const float scanlineIntensity = 0.1;
const float curvature = 0.1;
const float vignetting = 0.25;
const float colorAberration = 0.03;
const float noiseIntensity = 0.02;

vec3 applyScanlineEffect(vec3 color, float y) {
    float scanline = sin(y * 120.0 + time * 20.0) * scanlineIntensity;
    return color - vec3(scanline);
}

vec3 applyCurvature(vec3 color, vec2 uv) {
    vec2 center = uv - 0.5;
    float dist = length(center);
    vec2 distort = center * pow(dist, curvature);
    return texture(screenTexture, uv + distort).rgb;
}

vec3 applyVignetting(vec3 color, vec2 uv) {
    float dist = length(uv - 0.5);
    float vignette = smoothstep(0.8, vignetting, dist);
    return color * vignette;
}

vec3 applyColorAberration(vec3 color, vec2 uv) {
    float dist = length(uv - 0.5) * colorAberration;
    float r = texture(screenTexture, uv + vec2(dist, 0.0)).r;
    float g = texture(screenTexture, uv).g;
    float b = texture(screenTexture, uv - vec2(dist, 0.0)).b;
    return vec3(r, g, b);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 applyNoise(vec3 color, vec2 uv) {
    float noise = rand(uv + time) * noiseIntensity;
    return color + vec3(noise);
}

void main() {
    vec2 uv = TexCoords;

    // Apply curvature distortion
    vec3 color = applyCurvature(texture(screenTexture, uv).rgb, uv);

    // Apply color aberration
    color = applyColorAberration(color, uv);

    // Apply scanline effect
    color = applyScanlineEffect(color, uv.y);

    // Apply vignetting effect
    color = applyVignetting(color, uv);

    // Apply noise
    color = applyNoise(color, uv);

    FragColor = vec4(color, 1.0);
}
)";

