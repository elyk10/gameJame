// Fragment shader
#version 420

in vec2 fTextureCoords;

// for smoke particles
uniform bool bIsSmokeParticle;

uniform sampler2D texture_00;			// 2D meaning x,y or s,t or u,v
uniform sampler2D texture_01;

void main()
{
	//gl_FragDepth = gl_FragCoord.z;

	if (bIsSmokeParticle)
	{
		discard;
	}
}