#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform usampler2D boardTexture;
uniform sampler2D texA;
uniform sampler2D texB;
uniform sampler2D texC;
uniform int bWidth;
uniform int bHeight;

const vec4 domainAColor = vec4(0.0,0.0,0.8,1.0);
const vec4 domainBColor = vec4(0.0,0.8,0.0,1.0);
const vec4 domainCColor = vec4(0.8,0.0,0.0,1.0);

const vec4 backgroundColor = vec4(0.5,0.5,0.5,1.0);

void main() 
{
   vec4 BColor = texture(texB, TexCoord*3);
   vec4 CColor = texture(texC, TexCoord*10);
   vec4 AColor = texture(texA, TexCoord);

   float horizontalOffset = -(0.5/bWidth)*floor(bHeight * TexCoord.y);
   vec2 shiftedPosition = vec2(TexCoord.x + horizontalOffset, TexCoord.y);

   // RGBA [0,1]
   vec4 color;

   // This is the value (technically a shade of red) that is sampled to determine A,B, or C
   uint domainValue = texture(boardTexture, shiftedPosition).r;

   if(domainValue ==65u)
   {
      //color = domainAColor;
      color = AColor;
   }
   else if(domainValue ==66u)
   {
      //color = domainBColor;
      color = BColor;
   }
   else if(domainValue ==67u)
   {
      //color = domainCColor;
      color = CColor;
   }
   else if(domainValue ==46u)
   {
      color = backgroundColor;
   }
   else
   {
      color = vec4(0.1,0.0,0.1,1.0);
   }
   //color = vec4(float(domainValue)/70,0,0,1);
   //FragColor = mix(color,texture(texA,TexCoord),0.9);
   FragColor = color;
   //FragColor = AColor;
}