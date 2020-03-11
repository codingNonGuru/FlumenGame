#version 450

layout (location = 0) out vec4 fragment;

in vec2 pos;

void main()
{
    fragment = vec4(1.0f);

    float distance = pos.x * pos.x + pos.y * pos.y;
    float factor = exp(-distance / 0.05f);
    fragment.a = factor;
}