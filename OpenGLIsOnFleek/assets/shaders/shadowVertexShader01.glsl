// Vertex shader
#version 420

uniform mat4 matModel;			// Later...

uniform mat4 lightSpaceMatrix;


uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel_IT;		// Inverse transpose of the model matrix

uniform mat4 boneMatrices[150];
uniform bool bHasBones;


//uniform vec3 modelScale;
//uniform vec3 modelOffset;

in vec4 vPos;		
in vec4 vCol;
in vec4 vNormal;	
in vec2 vTextureCoords;		// NOTE this is a vec2 not vec4
in vec4 vTangent;
in vec4 vBitangent;
in vec4 vBoneIndex;
in vec4 vBoneWeight;

out vec2 fTextureCoords;

uniform bool bUseHeightMap;
uniform sampler2D heightMapSampler;		// Texture unit 20
uniform float heightScale;
uniform vec2 UVOffset;

uniform bool bIsGrass;
uniform vec4 windForce;
uniform vec4 grassPos;
uniform float timePassed;



void main()
{
	vec4 vertexModelPosition = vPos;	

	vec2 UVFinal = vTextureCoords.st;
	
	if ( bUseHeightMap )
	{
		// Greyscale (black and white) heightmap image
		// NvF5e_height_map.bmp
//		vec3 heightRGB = texture( heightMapSampler, vTextureCoords.st ).rgb;
//		float height = (heightRGB.r + heightRGB.g + heightRGB.b) / 3.0f;
		UVFinal += UVOffset.yx;
		float height = texture( heightMapSampler, UVFinal.st ).r;
		
		// Adjust the height of the y axis from this "colour"
//		const float SCALEADJUST = 25.0f;
//		vertexModelPosition.y += ( height * SCALEADJUST );
		vertexModelPosition.y += ( height * heightScale );
	}
	
	
	if (bIsGrass)
	{
		float height = vPos.y;

		float windStrength = sqrt(windForce.x * windForce.x + windForce.y * windForce.y + windForce.z * windForce.z);
		float windPower;
		windPower = 1.0f + sin(grassPos.x + grassPos.z/*vPos.x / 100.0f + vPos.z / 100.0f */+ timePassed * windStrength);
		vertexModelPosition += vec4(0.0f, 0.0f, 2.0f, 1.0f) * height * height * windPower;
		
		
		//windForce * height;
		vertexModelPosition.w = 1.0f;
		
	}

	//bone stuff
	if (bHasBones)
	{
		
		//vec4 totalPosition = vec4(0.0f);
		//for (int i = 0; i < 4; i++)
		//{
		//	vec4 localPosition = boneMatrices[int(vBoneIndex[i])] * vec4(vPos.xyz, 1.0f);
		//	totalPosition += localPosition * vBoneWeight[i];
		//}
		mat4 boneMatrix = boneMatrices[int(vBoneIndex[0])] * vBoneWeight[0];
		boneMatrix += boneMatrices[int(vBoneIndex[1])] * vBoneWeight[1];
		boneMatrix += boneMatrices[int(vBoneIndex[2])] * vBoneWeight[2];
		boneMatrix += boneMatrices[int(vBoneIndex[3])] * vBoneWeight[3];
		
		vertexModelPosition =  boneMatrix * vertexModelPosition;
		
	}
	
	vec4 totalPos = matModel * vertexModelPosition;
	gl_Position = lightSpaceMatrix * totalPos;//matModel * vertexModelPosition;//vec4(vertexModelPosition.xyz, 1.0f);

	fTextureCoords = vTextureCoords.st;
}
