#version 430
in vec2 TexCoord;
layout (location = 0) out vec4 FragColor;

layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D depth;

uniform int Set_post;
uniform int Set_dof;
uniform int Set_fxaa;
uniform vec2 Resolution;

#define KERNEL_SIZE 9

const float kernel[KERNEL_SIZE] = float[](
	0.0625, 0.1250, 0.0625,
	0.1250, 0.2500, 0.1250,
	0.0625, 0.1250, 0.0625
);

const vec2 offset[KERNEL_SIZE] = vec2[](
	vec2(-1.0,-1.0), vec2( 0.0,-1.0), vec2( 1.0,-1.0),
	vec2(-1.0, 0.0), vec2( 0.0, 0.0), vec2( 1.0, 0.0),
	vec2(-1.0, 1.0), vec2( 0.0, 1.0), vec2( 1.0, 1.0)
);

void main() {

	if(Set_dof == 1){
		vec2 pstep = vec2(2.0) / vec2(textureSize(Tex1, 0));
		vec4 res   = vec4(0.0);
		for (int i = 0; i < KERNEL_SIZE; ++i)
			res += texture(Tex1, TexCoord + offset[i] * pstep) * kernel[i];
		float blur_step = clamp(abs(texture(depth, TexCoord).r - 0.6), 0.0, 1.0);
		vec3 col = mix(texture(Tex1, TexCoord).rgb, res.rgb, blur_step * 3);
		FragColor = vec4(col, 1);
		return;
	}

	if(Set_post == 1){
		FragColor = vec4(vec3(1.0) - texture(Tex1, TexCoord).rgb, 1);
		return;
	}

	if(Set_fxaa == 1){
		vec2 u_texelStep = vec2(1.0 / Resolution.x, 1.0 / Resolution.y);
		float u_lumaThreshold = 0.6;
		float u_mulReduce = 1.0 / 8.0;
		float u_minReduce = 1.0 / 128.0;
		float u_maxSpan = 8.0;

		vec3 rgbM = texture(Tex1, TexCoord).rgb;
		vec3 rgbNW = textureOffset(Tex1, TexCoord, ivec2(-1, 1)).rgb;
		vec3 rgbNE = textureOffset(Tex1, TexCoord, ivec2(1, 1)).rgb;
		vec3 rgbSW = textureOffset(Tex1, TexCoord, ivec2(-1, -1)).rgb;
		vec3 rgbSE = textureOffset(Tex1, TexCoord, ivec2(1, -1)).rgb;
		const vec3 toLuma = vec3(0.299, 0.587, 0.114);
		float lumaNW = dot(rgbNW, toLuma);
		float lumaNE = dot(rgbNE, toLuma);
		float lumaSW = dot(rgbSW, toLuma);
		float lumaSE = dot(rgbSE, toLuma);
		float lumaM = dot(rgbM, toLuma);
		float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
		float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
		if (lumaMax - lumaMin < lumaMax * u_lumaThreshold) {
			FragColor = vec4(rgbM, 1);
			return;
		}
		
		vec2 samplingDirection ;
		samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
		samplingDirection.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
		float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);
		float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
		samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan, -u_maxSpan), vec2(u_maxSpan, u_maxSpan)) * u_texelStep;
		vec3 rgbSampleNeg = texture(Tex1, TexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
		vec3 rgbSamplePos = texture(Tex1, TexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;
		vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;
		vec3 rgbSampleNegOuter = texture(Tex1, TexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
		vec3 rgbSamplePosOuter = texture(Tex1, TexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
		vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;
		float lumaFourTab = dot(rgbFourTab, toLuma);
		if (lumaFourTab < lumaMin || lumaFourTab > lumaMax) {
			FragColor = vec4(rgbTwoTab, 1);
		} else {
			FragColor = vec4(rgbFourTab, 1);
		}
		return;
	}

	FragColor = vec4(texture(Tex1, TexCoord).rgb, 1);
}