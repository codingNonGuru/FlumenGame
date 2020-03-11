#version 450

layout (location = 0) out vec4 fragment;

uniform sampler2D roadAlpha;

in vec2 coords;

void main()
{
    fragment = vec4(1.0f);

    fragment.a = texture(roadAlpha, coords).r;
}