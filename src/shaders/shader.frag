#version 330 core

out vec4 FragColor;

uniform float SnakeBlockID;
uniform float numBlocks;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    float mult = map(SnakeBlockID, 0.0f, numBlocks, 1.0f, 0.0f);
    FragColor = mult * vec4(1.0f, 0.5f, 0.2f, 0.0f);
}