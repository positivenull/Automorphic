#version 400

uniform sampler2D inTex;
uniform sampler2D maskTex;

uniform float u_time;
uniform vec2  u_resolution;
uniform float u_blend;
uniform bool  u_invert;

out vec4 fragColor;

void main(){
    vec2 st = gl_FragCoord.xy / u_resolution.xy;

    vec4 texColor  = texture(inTex, st);
	vec4 maskColor = texture(maskTex, st);

	float blend = u_invert ? 1-u_blend : u_blend;
	float aMask = u_invert ? 1-maskColor.x : maskColor.x;
	float alpha = max(aMask, blend);


    fragColor = vec4(texColor.rgb, alpha);
}