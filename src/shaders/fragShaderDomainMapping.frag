#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform usampler2D boardTexture;
uniform sampler2D t1;
uniform int texWidth;
uniform int texHeight;

const vec3 domainAColor = vec3(0.0,0.0,0.8);
const vec3 domainBColor = vec3(0.0,0.8,0.0);
const vec3 domainCColor = vec3(0.8,0.0,0.0);

const vec3 backgroundColor = vec3(0.5,0.5,0.5);

void main() 
{
   float horizontalOffset = -(0.5/texWidth)*floor(texHeight * TexCoord.y);
   vec3 AColor = texture(t1, TexCoord).rgb;
   //vec3 AColor = texture(boardTexture, vec2(0.5,0.5)).rgb;
   vec2 shiftedPosition = vec2(TexCoord.x + horizontalOffset, TexCoord.y);
   vec3 color;
   uint domainValue = texture(boardTexture, shiftedPosition).r;
   if(domainValue ==65u)
   {
      color = domainAColor;
      //color = AColor;
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
   //FragColor = texture(boardTexture, TexCoord);
   //FragColor = texture(A_Texture, vec2(0.5,0.5));
   //FragColor = mix(texture(boardTexture,TexCoord),texture(t1,TexCoord),0.9);
   FragColor = vec4(color, 1.0);
}