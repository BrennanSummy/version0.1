#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform usampler2D ourTexture;

const vec3 domainAColor = vec3(0.0,0.0,0.8);
const vec3 domainBColor = vec3(0.0,0.8,0.0);
const vec3 domainCColor = vec3(0.8,0.0,0.0);

void main() 
{
   vec3 color;
   uint domainValue = texture(ourTexture, TexCoord).r;
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
   else
   {
      color = vec3(0.1,0.1,0.0);
   }
   //FragColor = texture(ourTexture, TexCoord);
   FragColor = vec4(color, 1.0);
}