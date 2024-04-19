// Fragment shader
#version 420

in vec4 fColour;
in vec4 fVertexWorldPos;			// vertex in "world space"
in vec4 fVertexWorldNormal;	
in vec2 fTextureCoords;			// u is x or red, v is y or green
in mat3 fTBN;
in vec4 fFragPosLight;

// These are listed in order, starting from zero
out vec4 outputColour;			// GL_COLOR_ATTACHMENT0      // To the frame buffer (aka screen)
out vec4 brightnessColour; 			// GL_COLOR_ATTACHMENT1
//out vec4 finalLightColour_and_Brightness;
//out vec4 vertexNormal;
//out vec4 motionSOmeting_Spec_SOmething;
//out vec4 vertexColour;
//
//out vec4 vertexWorldPositionXYZ_;
//out vec4 vertexWorldNormal;
//out vec4 vertexSpecular_and_Power;
//out vec4 vertexFinalColourRGB;

uniform sampler2D shadowMap;

//uniform vec3 directionalLightColour;
// rgb are the rgb of the light colour
//uniform vec4 directionalLight_Direction_power;
// xyz is the normalized direction, w = power (between 0 and 1)

// If true, then passes the colour without calculating lighting
uniform bool bDoNotLight;		// Really a float (0.0 or not zero)

uniform vec4 eyeLocation;

uniform bool bUseDebugColour;	// if this is true, then use debugColourRGBA for the colour
uniform vec4 debugColourRGBA;		

// If FALSE, we use the texture colour as the vertex colour
// (NOT the one from the model file)
uniform bool bUseVertexColours;		// If true, then DOESN'T use texture colours

// Usually, you would pass the alpha transparency as the 4th colour value, 
// 	but for clarity, we'll pass it separately...
uniform float transparencyAlpha;

uniform sampler2D texture_00;			// 2D meaning x,y or s,t or u,v
uniform sampler2D texture_01;
uniform sampler2D texture_02;
uniform sampler2D texture_03;
uniform sampler2D texture_04;			
uniform sampler2D texture_05;
uniform sampler2D texture_06;
uniform sampler2D texture_07;

//
uniform bool bUseHeightMap;
uniform sampler2D heightMapSampler;		// Texture unit 20
uniform sampler2D discardSampler;		// Texture unit 21

// Skybox, cubemap, etc.
uniform bool bIsSkyBox;
uniform samplerCube skyBoxTexture;

// nightvision
uniform bool bUseNightvision;

// For the discard example
uniform bool bUseDiscardMaskTexture;
uniform sampler2D maskSamplerTexture01;

// From the FBO
uniform bool bIsOffScreenTextureQuad;
uniform sampler2D textureOffScreen;
uniform vec2 screenWidthAndHeight;	// x is width

// dilation stuff
uniform bool dilateTexture;
uniform vec4 dilationValues; // x = size, y = separation, z = min threshold, w = max threshold
uniform sampler2D dilationTexture;

// depth texture values
uniform bool bIsDepthTexture;
uniform sampler2D depthTexture;

// depth of field values
uniform bool bUseDOF;
uniform vec4 DOFPosition;
uniform vec4 DOFValues; // x = focal length, y = bokeh scale

//... and so on
//uniform float textureMixRatio[8];
uniform vec4 textureMixRatio_0_3;		// 1, 0, 0, 0 
uniform vec4 textureMixRatio_4_7;

// for normal maps
uniform bool bHasNormalMap;

// for smoke particles
uniform bool bIsSmokeParticle;

// for blur
uniform bool bAddBloom;
uniform bool bGaussianBlur;
uniform bool bHorizontalBlur;
uniform float blurWeight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);
uniform sampler2D gaussianTexture;

//uniform samplerCube skyBox;

struct sLight
{
	vec4 position;			
	vec4 diffuse;	// Colour of the light (used for diffuse)
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 10;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 70 uniforms
//... is really:
//uniform vec4 theLights[0].position;
//uniform vec4 theLights[1].position;
//uniform vec4 theLights[2].position;
// etc...



vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );


// Some 2nd pass effects...
vec3 getFBOColour();
vec3 ChromicAberration( float amount );
vec3 BasicBlurScreen();
vec3 BlurScreen(int pixelOffset);
vec3 BlurScreenFaster(int pixelOffset);
float getDepthValue();
vec3 getDepthColour();
vec3 nightVision(vec3 originalColour);




void main()
{
//	gl_FragColor = vec4(color, 1.0);

//	if ( bUseHeightMap )
//	{
//		outputColour.rgba = vec4(1.0f, 0.0f, 0.0f, 1.0f);
//		return;
//	}

	// Discard the water
//	if ( bUseHeightMap )
//	{
//		// Range of 0 to 1
//		float height = texture( heightMapSampler, textureCoords.st ).r;
//		
//		if ( height <= 0.005f )
//		{
//			discard;
//		}
//	}
	
	
		// Offscreen quad
	if ( bIsOffScreenTextureQuad )
	{
		
//		outputColour.rgb = vec3(1.0f, 0.0f, 0.0f);
//		vec3 theColour = texture( textureOffScreen, fTextureCoords.st ).rgb;		
//		outputColour.rgb = theColour;						   
//		outputColour.a = 1.0f;
//		return;
//
		// Convert the integer pixel location (10, 15) or (1902, 546)
		// 	into a texture coordinate from 0.0 to 1.0
//		vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
//		                              gl_FragCoord.y / screenWidthAndHeight.y );
//		vec3 theColour = texture( textureOffScreen, textCoordsScreen.st ).rgb;	
//		vec3 HUDTextureColour = texture( textureHUF, textCoordsScreen.st).rgb;
//
//		outputColour.rgb = theColour * 0.5f + HUDTextureColour.rgb * 0.5f;
//
		// All of these assume we are sampling from the textureOffscreen sampler

//		outputColour.rgb = getFBOColour();
//		outputColour.rgb = ChromicAberration(0.5f);
//		outputColour.rgb = BasicBlurScreen();

		// 3 gives this: *** * *** = 7x7= 49
		// 5 gives this: ***** * ***** = 11x11 = 121 samples
//		outputColour.rgb = BlurScreen(25);

		//outputColour.rgb = BlurScreenFaster(5);
		if (dilateTexture)
		{
			int size = 3;//int(dilationValues.x);
			float separation = 2.0f;//dilationValues.y;
			float minThreshold = 0.1f;//dilationValues.z;
			float maxThreshold = 0.3f;//dilationValues.w;

			vec4 theBrightestColour = vec4(getFBOColour(), 1.0f);//texture(textureOffScreen, fTextureCoords.st);
			vec4 theColour = vec4(getFBOColour(), 1.0f);//theBrightestColour;//getFBOColour();//texture(textureOffScreen, fTextureCoords.st);
			float maxGreyValue = 0.0f;

			for (int i = -size; i <= size; i++)
			{
				for (int j = -size; j <= size; j++)
				{
					if (!(distance(vec2(i, j), vec2(0, 0)) <= size)) {continue;}

					vec2 textCoordsScreen = vec2( (gl_FragCoord.x + i * separation) / screenWidthAndHeight.x, 
									(gl_FragCoord.y + j * separation) / screenWidthAndHeight.y ); //+ (vec2(i, j) * separation);
		
					vec4 currentColour = texture( textureOffScreen, textCoordsScreen.st );
					//vec4 currentColour = texture(textureOffScreen, (fTextureCoords.st + (vec2(i, j) * separation)));

					float greyScaleValue = dot(currentColour.rgb, vec3(0.21f, 0.72f, 0.07f));

					//theColour = 
					if (greyScaleValue > maxGreyValue)
					{
						maxGreyValue = greyScaleValue;
						theBrightestColour = currentColour;
						
					}
					

				}
			}
			
			outputColour.rgb = mix(theColour.rgb, theBrightestColour.rgb, smoothstep(minThreshold, maxThreshold, maxGreyValue));
			return;
		}
		
		vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              gl_FragCoord.y / screenWidthAndHeight.y );
		if (bUseDOF)
		{
			
			float minDistance = 30.0f;
			float maxDistance = 40.0f;
			float dofDistance = 150.0f;
			
			
	
	//vec3 theColour = texture( textureOffScreen, textCoordsScreen.st ).rgb;
			vec4 colourInFocus = texture(textureOffScreen, textCoordsScreen.st);//vec4(getFBOColour(), 1.0f);
			vec4 colourOutOfFocus = texture(dilationTexture, textCoordsScreen.st);

			vec4 distanceFromCamera = texture(depthTexture, textCoordsScreen.st);

			float blur = smoothstep(minDistance, maxDistance, abs(distanceFromCamera.r* 255.0f - dofDistance));

			outputColour = mix(colourInFocus, colourOutOfFocus, blur);
			//outputColour = vec4(blur, 0.0f, 0.0f, 1.0f);
			
			return;
		}
		outputColour.rgb = getFBOColour();
		
		outputColour.a = 1.0f;

		if (bGaussianBlur)
		{
			vec3 blurColour = vec3(0.0f);
			if (bHorizontalBlur)
			{
				for (int i = -4; i <= 4; i++)
				{
					vec2 pixelUV = vec2 ((gl_FragCoord.x + i) / screenWidthAndHeight.x, 
											(gl_FragCoord.y ) / screenWidthAndHeight.y);
					blurColour += texture(textureOffScreen, pixelUV).rgb * blurWeight[abs(i)];
					//blurColour += texture(textureOffScreen, textCoordsScreen - pixelUV).rgb ;//* blurWeight[i];
				}
			}
			else
			{
				for (int i = -4; i <= 4; i++)
				{
					vec2 pixelUV = vec2 ((gl_FragCoord.x ) / screenWidthAndHeight.x, 
											(gl_FragCoord.y + i) / screenWidthAndHeight.y);
					blurColour += texture(textureOffScreen, pixelUV).rgb * blurWeight[abs(i)];
				}
			}
			outputColour = vec4(blurColour, 1.0f);
			
		}

		if (bAddBloom)
		{
			vec3 actualColour = outputColour.rgb;
			vec2 pixelUV = vec2(gl_FragCoord.x / screenWidthAndHeight.x, 
								gl_FragCoord.y / screenWidthAndHeight.y);
			vec3 bloomColour = texture(gaussianTexture, pixelUV).rgb;
			actualColour += bloomColour;
			vec3 result = vec3(1.0) - exp(-actualColour);
			result = pow(result, vec3(1.0f/1.5f));
			outputColour = vec4(result, 1.0f);
		}
		
		return;
	}

	
		

	if (bIsDepthTexture)
	{
		outputColour = vec4(sqrt(dot(eyeLocation - fVertexWorldPos,eyeLocation - fVertexWorldPos))/255.0f);
		return;
	}

	if (bUseDOF)
	{
		vec4 theColour = texture(textureOffScreen, fTextureCoords.st);
		float depth = texture(texture_00, fTextureCoords.st).r;
		
		float focalDistance = sqrt(dot(eyeLocation - DOFPosition,eyeLocation - DOFPosition));
		float focalLength = DOFValues.x;
		float signedDistance = depth - focalDistance;
		float magnitude = smoothstep(0.0f, focalLength, abs(signedDistance));
		float CoC = magnitude;

		if (CoC > 0.0f)
		{
			outputColour = vec4(0.0f, (step(signedDistance, 0.0) * CoC) / 255.0f, 0.0f, 1.0f);
		}
		else
		{
			outputColour = vec4(1.0f, 0.0f, (step(signedDistance, 0.0) * CoC) / 255.0f, 1.0f);
		}

		return;
	}

	
	if ( bUseDiscardMaskTexture )
	{
		float maskValue = texture( maskSamplerTexture01, fTextureCoords.st ).r;
		// If "black" then discard
		if ( maskValue < 0.1f )
		{
			discard;
//			outputColour.rgba = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
//			return;
		}	
	}
	
	if ( bIsSkyBox )
	{
		//uniform samplerCube skyBoxTexture;
		vec4 skyBoxSampleColour = texture( skyBoxTexture, fVertexWorldNormal.xyz ).rgba;
		outputColour.rgb = skyBoxSampleColour.rgb;
		outputColour.a = 1.0f;
		return;
	}
	
	// Reflect
	// Reflects based on the eye position
	// genType reflect( genType IncidentAngle, 	genType Nnormal);
//	vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);
//	vec3 reflectAngle = reflect( eyeVector, vertexWorldNormal.xyz);
//	vec3 reflectAngle = refract( eyeVector, vertexWorldNormal.xyz, 	1.333);
//	
//	vec4 skyBoxSampleColour = texture( skyBoxTexture, reflectAngle.xyz ).rgba;
	
//	outputColour.rgb = skyBoxSampleColour.rgb;
//	outputColour.a = 1.0f;
//	return;
	

	vec4 textureColour = 
			  texture( texture_00, fTextureCoords.st ).rgba * textureMixRatio_0_3.x 	
			+ texture( texture_01, fTextureCoords.st ).rgba * textureMixRatio_0_3.y
			+ texture( texture_02, fTextureCoords.st ).rgba * textureMixRatio_0_3.z
			+ texture( texture_03, fTextureCoords.st ).rgba * textureMixRatio_0_3.w;

	// Make the 'vertex colour' the texture colour we sampled...
	vec4 vertexRGBA = textureColour;	
	
	
	// ...unless we want to use the vertex colours from the model
	if (bUseVertexColours)
	{
		// Use model vertex colour and NOT the texture colour
		vertexRGBA = fColour;
	}
	
	if ( bUseDebugColour )
	{	
		vertexRGBA = debugColourRGBA;
	}
	
	if ( bDoNotLight )
	{
		outputColour = vertexRGBA;
		float brightnessValue = dot(outputColour.rgb, vec3(0.2126, 0.7152, 0.0722));
		if (brightnessValue > 0.98f)
		{
			brightnessColour = vec4(outputColour.rgb, 1.0f);
		}
		else
		{
			brightnessColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		return;
	}
	

	// *************************************
	// Hard code the specular (for now)
	vec4 vertexSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// xyzw or rgba or stuw
	// RGB is the specular highglight colour (usually white or the colour of the light)
	// 4th value is the specular POWER (STARTS at 1, and goes to 1000000s)
	
	vec4 fragNorm = fVertexWorldNormal;
	if (bHasNormalMap)
	{
		fragNorm = vec4((texture(texture_01, fTextureCoords.st).rgb), 1.0f);
		fragNorm = fragNorm * 2.0f - 1.0f;
		fragNorm = vec4(normalize(fTBN * fragNorm.xyz), 1.0f);
	}

	vec4 vertexColourLit = calculateLightContrib( vertexRGBA.rgb, fragNorm.xyz,//fVertexWorldNormal.xyz, 
	                                              fVertexWorldPos.xyz, vertexSpecular );
	// *************************************
			
	outputColour.rgb = vertexColourLit.rgb;
	
	// Real gamma correction is a curve, but we'll Rock-n-Roll it here
	outputColour.rgb *= 1.35f;
	
	// This is where the alpha transparency happens
//	outputColour.a = 1.0f;
	outputColour.a = transparencyAlpha;
	
	float brightnessValue = dot(outputColour.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightnessValue > 0.98f && !bIsSmokeParticle)
	{
		brightnessColour = vec4(outputColour.rgb, 1.0f);
	}
	else
	{
		brightnessColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	if (bIsSmokeParticle)
	{
		//vec4 textureColour = texture( texture_00, fTextureCoords.st ).rgba * textureMixRatio_0_3.x;
		outputColour.a = texture( texture_01, fTextureCoords.st ).r * transparencyAlpha;
		//vertexRGBA = textureColour;
		//return;
	}
	if (bUseNightvision)
	{
		//outputColour.rgb = getFBOColour();

		outputColour.rgb = nightVision(outputColour.rgb);
		
						   
		//outputColour.a = 1.0f;
		return;
	}
	
	
	//outputColour = vec4(sqrt(dot(eyeLocation - fVertexWorldPos,eyeLocation - fVertexWorldPos))/255.0f);
	
	// DEBUG HACK
//	outputColour.rgb *= 0.0001f;
//	outputColour.rgb += vec3( textureCoords.st, 0.0f);
}

// Does nothing, just samples the FBO texture
vec3 getFBOColour()
{
	vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              gl_FragCoord.y / screenWidthAndHeight.y );
	
	vec3 theColour = texture( textureOffScreen, textCoordsScreen.st ).rgb;	
	
	return theColour;
}

vec3 nightVision(vec3 originalColour)
{
	originalColour.rgb *= vec3(0.0f, 1.0f, 0.0f);
	originalColour.rgb += 0.2f;
	originalColour.rgb = pow(originalColour.rgb, vec3(3.0f));

	vec4 HUDtextureColour = texture( texture_01, fTextureCoords.st//vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              //gl_FragCoord.y / screenWidthAndHeight.y ) 
								  ).rgba * textureMixRatio_0_3.x;

	if (HUDtextureColour.r > 0.1f)
	{
		originalColour.rgb = HUDtextureColour.rgb;
	}

	return originalColour;
}

vec3 getDepthColour()
{
	//vec2 textCoordsScreen = vec2( fTextureCoords.st);//gl_FragCoord.x / screenWidthAndHeight.x, 
	                              //gl_FragCoord.y / screenWidthAndHeight.y );
	
	vec3 theColour = texture( depthTexture, fTextureCoords.st ).rgb;	
	
	return theColour;
}

float getDepthValue()
{
	vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              gl_FragCoord.y / screenWidthAndHeight.y );
	float theDepth = texture(depthTexture, textCoordsScreen.st).r;

	return theDepth;
}

vec3 ChromicAberration( float amount )
{
	vec2 textCoordsScreen = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              gl_FragCoord.y / screenWidthAndHeight.y );
					
	vec3 theColour = vec3(0.0f);
					
	// Red coordinate is off up (-0.025 in y) and to the left (-0.01f in x)
	vec2 redYV = vec2( textCoordsScreen.x - (amount * 0.01f),
					   textCoordsScreen.y - (amount * 0.025f));					   
	theColour.r = texture( textureOffScreen, redYV ).r;	
	
	// Green coordinate is off up (+0.002 in y) and to the right (+0.01f in x)
	vec2 greenYV = vec2( textCoordsScreen.x + (amount * 0.01f),
					     textCoordsScreen.y + (amount * 0.002f));					   
	theColour.g = texture( textureOffScreen, greenYV ).g;	
	
	// Green coordinate is off the left (-0.015f in x)
	vec2 blueYV = vec2( textCoordsScreen.x - (amount * 0.015f),
					     textCoordsScreen.y);					   
	theColour.b = texture( textureOffScreen, blueYV ).b;	
	
	return theColour;
}

vec3 BasicBlurScreen()
{
	//   *
	// * O *
	//   *
	// "O" is the pixel we are "on"
	// * are the adjacent pixels
	
	vec2 pixelUV = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                              gl_FragCoord.y / screenWidthAndHeight.y );
					
	vec3 theColourPixel =  texture( textureOffScreen, pixelUV ).rgb;	
				
	// To the left (-ve in X)
	vec2 pixel0 = vec2( (gl_FragCoord.x - 1) / screenWidthAndHeight.x, 
	                     gl_FragCoord.y / screenWidthAndHeight.y );					
	vec3 pixel0Colour =  texture( textureOffScreen, pixel0 ).rgb;	

	// To the right (+ve in X)
	vec2 pixel1 = vec2( (gl_FragCoord.x + 1) / screenWidthAndHeight.x, 
	                     gl_FragCoord.y / screenWidthAndHeight.y );					
	vec3 pixel1Colour =  texture( textureOffScreen, pixel1 ).rgb;	
	
	// Above (-ve in y)
	vec2 pixel2 = vec2(  gl_FragCoord.x / screenWidthAndHeight.x, 
	                     (gl_FragCoord.y - 1) / screenWidthAndHeight.y );					
	vec3 pixel2Colour =  texture( textureOffScreen, pixel2 ).rgb;	
	
	// Below (+ve in y)
	vec2 pixel3 = vec2(  gl_FragCoord.x / screenWidthAndHeight.x, 
	                     (gl_FragCoord.y + 1) / screenWidthAndHeight.y );					
	vec3 pixel3Colour =  texture( textureOffScreen, pixel3 ).rgb;	
	
	// There are five (5) pixels so average by dividing by 5
	vec3 finalColour = vec3(0.0f);
	finalColour += (theColourPixel + pixel0Colour + pixel1Colour + pixel2Colour + pixel3Colour) / 5.0f;

	return finalColour;
}

vec3 BlurScreen(int pixelOffset)
{
	// pixelOffset = 1   pixelOffset = 2
	// 3x3               5x5
	// * * *             * * * * * 
	// * O *             * * * * * 
	// * * *             * * O * * 
	//                   * * * * * 
	//                   * * * * * 
	// 
	// "O" is the pixel we are "on"
	// * are the adjacent pixels
	
	vec3 outColour = vec3(0.0f);
	int totalSamples = 0;
	
	for ( int xOffset = -pixelOffset; xOffset <= pixelOffset; xOffset++ )
	{
		for ( int yOffset = -pixelOffset; yOffset <= pixelOffset; yOffset++ )
		{
			totalSamples++;
			
			vec2 pixelUV = vec2( (gl_FragCoord.x + xOffset) / screenWidthAndHeight.x, 
	                             (gl_FragCoord.y + yOffset) / screenWidthAndHeight.y );
					
			outColour += texture( textureOffScreen, pixelUV ).rgb;	
		}	
	}

	// Average this colour by the number of samples
	outColour.rgb /= float(totalSamples);

	return outColour;
}

float ShadowCalc(vec4 fragPosLightSpace, float bias)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 0.0f;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
			if (currentDepth - bias > pcfDepth)
			{
				shadow += 1.0f;
			}
		}
	}
	
	shadow = shadow/9.0f;
	shadow = shadow - 0.2f;
	/*if (currentDepth - bias > closestDepth)
	{
		shadow = 1.0f;
	}
*/
	if (projCoords.z > 1.0f)
	{
		shadow = 0.0f;
	}

	return shadow;
}

vec3 BlurScreenFaster(int pixelOffset)
{
	// pixelOffset = 1   pixelOffset = 2
	// 3x3               5x5
	//   *                   * 
	// * O *                 * 
	//   *               * * O * * 
	//                       *  
	//                       * 
	// 
	// "O" is the pixel we are "on"
	// * are the adjacent pixels
	
	vec3 outColour = vec3(0.0f);
	int totalSamples = 0;
	
	// Add up a horizontal sample
	for ( int xOffset = -pixelOffset; xOffset <= pixelOffset; xOffset++ )
	{
		totalSamples++;
			
		vec2 pixelUV = vec2( (gl_FragCoord.x + xOffset) / screenWidthAndHeight.x, 
	                          gl_FragCoord.y / screenWidthAndHeight.y );
					
		outColour += texture( textureOffScreen, pixelUV ).rgb;	
	}
	
	// Add up a vertical sample
	for ( int yOffset = -pixelOffset; yOffset <= pixelOffset; yOffset++ )
	{
		totalSamples++;
			
		vec2 pixelUV = vec2( gl_FragCoord.x / screenWidthAndHeight.x, 
	                         (gl_FragCoord.y + yOffset) / screenWidthAndHeight.y );
					
		outColour += texture( textureOffScreen, pixelUV ).rgb;	
	}	


	// Average this colour by the number of samples
	outColour.rgb /= float(totalSamples);

	return outColour;
}

vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			vec3 ambient = 0.0f * lightContrib;
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			vec3 viewDir = normalize(eyeLocation.xyz - fVertexWorldPos.xyz);
			float spec = 0.0f;
			vec3 halfwayDir = normalize(-theLights[index].direction.xyz + viewDir);
			spec = pow(max(dot(norm, halfwayDir), 0.0f), 64.0f);
			vec3 specular = spec * theLights[index].diffuse.rgb;

			float bias = max(0.001f * (1.0 - dot(norm, theLights[index].direction.xyz)), 0.0005f);
			float shadow = ShadowCalc(fFragPosLight, bias);
			
			
			finalObjectColour.rgb += (ambient + (1.0 - shadow) * (lightContrib) * vertexMaterialColour.rgb);			
			//finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			return finalObjectColour;		
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}


//	// For now, just trust Michael
//	// Very basic directional shader
//	vec3 lightContrib = directionalLightColour * directionalLight_Direction_power.w;
//	// 
//	// Get the dot product of the light and normalize
//	float dotProduct = dot( -directionalLight_Direction_power.xyz,  
//							vertexWorldNormal.xyz );	
//	// Clamp this to a positive number
//	dotProduct = max( 0.0f, dotProduct );		// 0 to 1		
//	
//	lightContrib *= dotProduct;		