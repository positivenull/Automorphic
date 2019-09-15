#version 410

uniform sampler2D tex0;
uniform sampler2D tex1;
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

    //fragColor = vec4(texColor.rgb, alpha);
	fragColor = mix(maskColor, texColor, u_blend);
}