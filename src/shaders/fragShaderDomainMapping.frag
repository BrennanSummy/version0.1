#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform usampler2D ourTexture;
uniform int texWidth;
uniform int texHeight;

const vec3 domainAColor = vec3(0.0,0.0,0.8);
const vec3 domainBColor = vec3(0.0,0.8,0.0);
const vec3 domainCColor = vec3(0.8,0.0,0.0);

const vec3 backgroundColor = vec3(0.0,0.0,0.0);

void main() 
{
   //float horizontalOffset = -0.125*floor(3 * TexCoord.y);
   float horizontalOffset = -(0.5/texWidth)*floor(texHeight * TexCoord.y);
   vec2 shiftedPosition = vec2(TexCoord.x + horizontalOffset, TexCoord.y);
   vec3 color;
   //uint domainValue = texture(ourTexture, TexCoord).r;
   uint domainValue = texture(ourTexture, shiftedPosition).r;
   if(domainValue ==65u)
   {
      color = domainAColor;
   }
   else if(domainValue ==66u)
   {
      color = domainBColor;
   }
   else if(domainValue ==67u)
   {
      color = domainCColor;
   }
   else if(domainValue ==46u)
   {
      color = backgroundColor;
   }
   else
   {
      color = vec3(0.3,0.0,0.3);
   }
   //FragColor = texture(ourTexture, TexCoord);
   FragColor = vec4(color, 1.0);
}