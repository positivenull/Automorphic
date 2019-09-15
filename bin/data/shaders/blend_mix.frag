#version 410

uniform sampler2D inTex;
uniform sampler2D maskTex;

uniform float u_time;
uniform vec2  u_resolution;
uniform float u_blend;

out vec4 fragColor;

void main(){
    vec2 st = gl_FragCoord.xy / u_resolution.xy;

    vec4 texColor  = texture(inTex, st);
	vec4 maskColor = texture(maskTex, st);
	float alpha = maskColor.x;
	
	vec3 col = mix(maskColor.rgb, texColor.rgb, u_blend);

	fragColor = vec4(col, alpha)
}