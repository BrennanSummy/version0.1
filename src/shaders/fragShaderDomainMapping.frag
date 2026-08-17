#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

out vec4 FragColor;

in vec2 TexCoord;

uniform usampler2D boardTopTexture;
uniform sampler2D texA;
uniform sampler2D texB;
uniform sampler2D texC;
uniform usampler2D boardBotTexture;
uniform int bWidth;
uniform int bHeight;
layout(location = 4) uniform bool showTop;
layout(location = 5) uniform bool showBot;

const vec4 domainAColor = vec4(0.0,0.0,0.8,1.0);
const vec4 domainBColor = vec4(0.0,0.8,0.0,1.0);
const vec4 domainCColor = vec4(0.8,0.0,0.0,1.0);

const vec4 backgroundColor = vec4(0.5,0.5,0.5,1.0);

void main() 
{
   vec4 AColor = texture(texA, TexCoord*bHeight);
   vec4 BColor = texture(texB, TexCoord*bHeight);
   vec4 CColor = texture(texC, TexCoord*bHeight);

   float horizontalOffset = -(0.5/bWidth)*floor(bHeight * TexCoord.y);
   vec2 shiftedPosition = vec2(TexCoord.x + horizontalOffset, TexCoord.y);

   // RGBA [0,1]
   vec4 topColor;
   vec4 botColor;

   // This is the value (technically a shade of red) that is sampled to determine A,B, or C
   uint topDomainValue = texture(boardTopTexture, shiftedPosition).r;
   uint botDomainValue = texture(boardBotTexture, shiftedPosition).r;

   if(topDomainValue ==65u)
   {
      topColor = domainAColor;
      //color = AColor;
   }
   else if(topDomainValue ==66u)
   {
      topColor = domainBColor;
      //color = BColor;
   }
   else if(topDomainValue ==67u)
   {
      topColor = domainCColor;
      //color = CColor;
   }
   else if(topDomainValue ==46u)
   {
      topColor = backgroundColor;
   }
   else
   {
      topColor = vec4(0.1,0.0,0.1,1.0);
   }
   if(botDomainValue ==65u)
   {
      botColor = domainAColor;
      //color = AColor;
   }
   else if(botDomainValue ==66u)
   {
      botColor = domainBColor;
      //color = BColor;
   }
   else if(botDomainValue ==67u)
   {
      botColor = domainCColor;
      //color = CColor;
   }
   else if(botDomainValue ==46u)
   {
      botColor = backgroundColor;
   }
   else
   {
      botColor = vec4(0.1,0.0,0.1,1.0);
   }
   if(showTop && showBot)
   {
   FragColor = mix(topColor,botColor,0.5);
   }
   else if(showTop && !showBot)
   {
   FragColor = topColor;
   }
   else if(showBot && !showTop)
   {
   FragColor = botColor;
   }
   else
   {
   FragColor = backgroundColor;
   }
}