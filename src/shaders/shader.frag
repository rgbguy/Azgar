#version 330 core

out vec4 FragColor;

uniform float SnakeBlockID;

void main()
{
    FragColor = (5.0f-SnakeBlockID) * vec4(0.5f, 0.5f, 0.2f, 0.0f)/5.0f + vec4(0.1f, 0.1f, 0.5f, 1.0f);
}