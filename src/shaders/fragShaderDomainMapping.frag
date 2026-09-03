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

// Top board colors
// Red
const vec4 domainATopColor = vec4(0.82,0.21,0.12,1.0);//vec4(1.0,0.78,0.0,1.0); //vec4(0.0,0.0,0.8,1.0);
// Green
const vec4 domainBTopColor = vec4(0.06,0.76,0.14,1.0); //vec4(0.0,0.8,0.0,1.0);
// Blue
const vec4 domainCTopColor = vec4(0.06,0.3,0.66,1.0); //vec4(0.8,0.0,0.0,1.0);
// Bottom board colors
// Red
const vec4 domainABotColor = vec4(0.82,0.21,0.12,1.0);//vec4(0.06,0.66,0.34,1.0);//vec4(0.65,0.87,0.75,1.0);
// Green
const vec4 domainBBotColor = vec4(0.06,0.76,0.14,1.0);//vec4(0.95,0.31,0.12,1.0);
// Blue                    
const vec4 domainCBotColor = vec4(0.06,0.3,0.66,1.0);// vec4(0.49,0.1,1.0,1.0);

// Gray
const vec4 backgroundColor = vec4(0.5,0.5,0.5,1.0);

void main() 
{
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
      topColor = domainATopColor;
      //color = AColor;
   }
   else if(topDomainValue ==66u)
   {
      topColor = domainBTopColor;
      //color = BColor;
   }
   else if(topDomainValue ==67u)
   {
      topColor = domainCTopColor;
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
      botColor = domainABotColor;
      //color = AColor;
   }
   else if(botDomainValue ==66u)
   {
      botColor = domainBBotColor;
      //color = BColor;
   }
   else if(botDomainValue ==67u)
   {
      botColor = domainCBotColor;
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
   FragColor = mix(topColor,botColor,0.2);
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