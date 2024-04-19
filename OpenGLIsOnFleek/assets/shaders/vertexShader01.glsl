// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;			// Later...
uniform mat4 matModel_IT;		// Inverse transpose of the model matrix

uniform mat4 boneMatrices[150];
uniform bool bHasBones;

uniform mat4 lightSpaceMatrix;

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

//out vec4 fColour;
//out vec4 fVertexWorldPos;	
//out vec4 fVertexWorldNormal;
//out vec2 fTextureCoords;		// ADDED

out vec4 gColour;
out vec4 gVertexWorldPos;	
out vec4 gVertexWorldNormal;
out vec2 gTextureCoords;		// ADDED
out mat3 gTBN;
out vec4 gFragPosLight;

uniform bool bHasNormalMap;

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
	//mat3 TBN;
	
	if (bHasNormalMap)
	{
		vec3 T = normalize(vec3(matModel * vTangent));
		vec3 B = normalize(vec3(matModel * vBitangent));
		vec3 N = normalize(vec3(matModel * vNormal));
		gTBN = mat3(T, B, N);
	}
	
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
		mat4 boneMatrix = boneMatrices[int(vBoneIndex[0])] * vBoneWeight[0];
		boneMatrix += boneMatrices[int(vBoneIndex[1])] * vBoneWeight[1];
		boneMatrix += boneMatrices[int(vBoneIndex[2])] * vBoneWeight[2];
		boneMatrix += boneMatrices[int(vBoneIndex[3])] * vBoneWeight[3];
		
		vertexModelPosition =  boneMatrix * vertexModelPosition;
		
	}
	
//	gl_Position = MVP * vec4(finalPos, 1.0);
//	gl_Position = MVP * vertModelPosition;
	
	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vertexModelPosition;
	
		
	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	gVertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	gVertexWorldNormal.xyz = normalize(gVertexWorldNormal.xyz);
	gVertexWorldNormal.w = 1.0f;
	
	
	gVertexWorldPos = matModel * vec4( vertexModelPosition.xyz, 1.0f);
		
	gFragPosLight = lightSpaceMatrix * vec4(gVertexWorldPos.xyz, 1.0f);
	
	gColour = vCol;

	
	
	// Copy the UV coordinates unchanged (to the fragment shader)
//	textureCoords = vTextureCoords;
	gTextureCoords = UVFinal;
}
