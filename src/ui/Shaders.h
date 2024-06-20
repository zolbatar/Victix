#pragma once

const char *blur_vertex_shader_src = R"(
    in vec2 inPos;
    in vec2 inTexCoord;
    out vec2 TexCoord;

    void main() {
        gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
        TexCoord = inTexCoord;
    }
)";

const char *blur_fragment_shader_src = R"(
    in vec2 TexCoord;
    uniform sampler2D image;
    uniform bool horizontal;
    uniform float weight[5];
    out vec4 FragColor;

    void main() {
        vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
        vec3 result = texture(image, TexCoord).rgb * weight[0]; // current fragment's contribution

        if (horizontal) {
            for (int i = 1; i < 5; ++i) {
                result += texture(image, TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
                result += texture(image, TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            }
        } else {
            for (int i = 1; i < 5; ++i) {
                result += texture(image, TexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
                result += texture(image, TexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            }
        }

        FragColor = vec4(result, 1.0);
    }
)";

const char *final_fragment_shader_source = R"(
        in vec2 TexCoord;
        uniform sampler2D horizontalBlurredImage;
        uniform sampler2D verticalBlurredImage;
        out vec4 FragColor;
        void main() {
FragColor = texture(horizontalBlurredImage, TexCoord);
/*            vec3 color = texture(horizontalBlurredImage, TexCoord).rgb + texture(verticalBlurredImage, TexCoord).rgb;
            FragColor = vec4(color, 1.0);*/
        }
    )";

