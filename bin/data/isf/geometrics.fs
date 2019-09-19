/*
{
  "CREDIT": "by nicolas escarpentier",
  "CATEGORIES" : [
  ],
  "DESCRIPTION" : "",
  "INPUTS" : [
  {
    "NAME" :      "u_gridSize",
    "TYPE" :      "float",
    "DEFAULT" :   5.0,
    "MIN" :       1.0,
    "MAX" :       8.0
  },
  {
    "NAME" :      "u_moveSpeed",
    "TYPE" :      "float",
    "DEFAULT" :   1,
    "MIN" :       0.1,
    "MAX" :       10.0
  },
  {
    "NAME" :		  "u_colorSpeed",
    "TYPE" : 		  "float",
    "DEFAULT" : 	0.1,
    "MIN" : 		  0.0,
    "MAX" :			  1.8
  },
  {
     "NAME" :		  "u_colorWave",
     "TYPE" :		  "float",
     "DEFAULT" :	0.1,
     "MIN" : 		  0.0,
     "MAX" :		  3.0	
  },
  {
    "NAME" :      "u_intensity",
    "TYPE" :      "float",
    "DEFAULT" :   1.0,
    "MIN" :       0.1,
    "MAX" :       5.0
  },
  {
     "NAME" :		  "u_blend",
     "TYPE" :		  "float",
     "DEFAULT" :	0.0,
     "MIN" : 		  0.0,
     "MAX" :		  1.0	
   }
 ],
   "ISFVSN" : 2.0
}
*/



// ===========================================================================
// === RGB <> HSV ============================================================
// ===========================================================================
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
    vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


// ===========================================================================
// === CRYSTAL MASK ==========================================================
// ===========================================================================
float wave_c(in vec2 _uv, in float _angle) {
    vec2 rotation = vec2(cos(_angle), sin(_angle));
    return cos(dot(_uv, rotation));
}
float wave_s(in vec2 _uv, in float _angle) {
    vec2 rotation = vec2(sin(_angle), cos(_angle));
    return cos(dot(_uv, rotation));
}

float wrap(in float x){ return abs(mod(x, 2.)-1.); }


// ===========================================================================
// === VORONOI ===============================================================
// === based on : shadertoy.com\/view\/4lBSzW by Shane =======================
// ===========================================================================

// Standard 2x2 hash algorithm.
vec2 hash22(vec2 p) { 
    // Faster, but probably doesn't disperse things as nicely as other ways.
    float n = sin(dot(p,vec2(1, 113))); 
    p = fract(vec2(8.*n, n)*262144.);
    return sin(p*6.2831853 + TIME*2.);
}

float Voronoi3Tap(vec2 p){
  vec2 s = floor(p + (p.x + p.y)*.3660254); // Skew the current point.
  p -= s - (s.x + s.y)*.2113249; // Use it to attain the vector to the base vertice (from p).

  // Determine which triangle we're in -- Much easier to visualize than the 3D version.
  // The following is equivalent to "float i = step(p.y, p.x)," but slightly faster, I hear.
  float i = p.x<p.y ? 0. : 1.;
  
  // Vectors to the other two triangle vertices.
  vec2 p1 = p - vec2(i, 1. - i) + .2113249, p2 = p - .5773502; 

  // Add some random gradient offsets to the three vectors above.
  p += hash22(s)*.125;
  p1 += hash22(s +  vec2(i, 1. - i))*.125;
  p2 += hash22(s + 1.)*.125;
  
  // Determine the minimum Euclidean distance. You could try other distance metrics, if you wanted.
  float d = min(min(dot(p, p), dot(p1, p1)), dot(p2, p2))/.425;
  
  // That's all there is to it.
  return sqrt(d); // Take the square root, if you want, but it's not mandatory.
}


// ===========================================================================
// === COLOR WALL ============================================================
// === based on : glslsandbox/e#22020.0 ======================================
// ===========================================================================

#define glow_size 5.0
#define glow_br   0.2
#define time_step 10.0

float random(vec2 seed) {
	float value = fract(sin(dot(seed ,vec2(12.9898,78.233))) * 43758.5453);
	return value;
}

float step_to(float value, float steps) {
	float  closest_int = floor(value / steps);
	return closest_int * steps;
}

vec4 dot_grid(vec2 pos, bool with_grid) {
	float grid_size = u_gridSize * 3.;
	float value = floor(mod(pos.x,grid_size)) * floor(mod(pos.y,grid_size));		
	value		= clamp(value, 0.0, 1.0);
	
	float c_time	= TIME / time_step;
	
	vec2 step_pos	= vec2(step_to(pos.x , grid_size), step_to(pos.y, grid_size));
	vec2 norm_pos	= step_pos.xy / RENDERSIZE.xy;
	
	norm_pos	= vec2(norm_pos.x + random(norm_pos), norm_pos.y + random(norm_pos ));
	
	float r = fract(sin(norm_pos.x ));
	float g = fract(sin(norm_pos.y + abs(c_time) ));
	float b = abs(r-g);
	
	if(with_grid == false)
	{
		value = 1.0;	
	}
	
	return vec4(r,g,b,1.0) * value;
}

vec4 glow(vec2 pos) {
	vec4 color 	=  clamp(dot_grid(pos, true) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x - u_intensity,pos.y),false) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x + u_intensity,pos.y),false) * glow_br, 0.0, 1.0);
	
	color		+= clamp(dot_grid(vec2(pos.x,pos.y - u_intensity),false ) * glow_br, 0.0, 1.0);
	color		+= clamp(dot_grid(vec2(pos.x,pos.y + u_intensity),false ) * glow_br, 0.0, 1.0);
	
	return color;
}


// ===========================================================================
// === MAIN ==================================================================
// ===========================================================================

void main() {
  // Screen coordinates.
	vec2 uv = (gl_FragCoord.xy - RENDERSIZE.xy * 0.5) / RENDERSIZE.y;
	uv.y += 0.01 * u_moveSpeed * u_moveSpeed * TIME;
	uv *= u_gridSize;

	vec2 st = gl_FragCoord.xy / RENDERSIZE.xy - vec2(0.5);
  st.y += 0.03;
	st = st * (1 + vec2(u_gridSize - 4.)/4);

  
  // === VORONOI ===============================================================
  // Take two 3-tap Voronoi samples near one another.
  float c = Voronoi3Tap(uv * 5.);
  float c2 = Voronoi3Tap(uv * 5. - 9./RENDERSIZE.y);
  
  // Coloring the cell.
  // Use the Voronoi value, "c," above to produce a couple of different colors.
  // Mix those colors according to some kind of moving geometric patten.
  // Setting "pattern" to zero or one displays just one of the colors.
  float pattern = cos(uv.x*.75*3.14159 - .9)*cos(uv.y*1.5*3.14159 - .75)*.5 + .5;
  
  // Just to confuse things a little more, two different color schemes are faded in out.
  // Color scheme one - Mixing a firey red with some bio green in a sinusoidal kind of pattern.
  vec3 col = mix(vec3(c*1.3, c*c, pow(c, 10.)), vec3(c*c*.8, c, c*c*.35), pattern );
  // Color scheme two - Farbrausch fr-025 neon, for that disco feel. :)
  vec3 col2 = mix(vec3(c*1.2, pow(c, 8.), c*c), vec3(c*1.3, c*c, pow(c, 10.)), pattern );
  // Alternating between the two color schemes.
  col = mix(col, col2, smoothstep(.4, .6, sin(TIME*.25)*.5 + .5)); // 
  
  // Hilighting.
  // Use a combination of the sample difference to add some really cheap, blueish highlighting.
  // It's a directional-derviative based lighting trick. Interesting, but no substitute for point-lit
  // bump mapping. Comment the following line out to see the regular, flat pattern.
  col += vec3(.5, .8, 1)*(c2*c2*c2 - c*c*c)*5.;
      
  // Speckles.
  // Adding subtle speckling to break things up and give it a less plastic feel.
  col += (length(hash22(uv + TIME))*.06 - .03)*vec3(1, .5, 0);
  
  // Rectangular Vignette.
  uv = gl_FragCoord.xy/RENDERSIZE.xy;
  col *= smoothstep(0., .5, pow(16.*uv.x*uv.y*(1. - uv.x)*(1. - uv.y), .25))*vec3(1.1, 1.07, 1.01);
  
  // Approximate gamma correction.
  col = sqrt(max(col, 0.));

  
  // === COLOR WALL ============================================================
  vec3 wallCol = glow(uv*RENDERSIZE.xy).rgb;


  // === CRYSTAL MASK ==========================================================
  float bri_wave = 0.;
  for(float i=4.; i<=8.; i++){
      bri_wave += wave_c(st*50., TIME / i);
      bri_wave += wave_c(st*50., i);
  }
  bri_wave += wave_s(st*80., -TIME/2.);
  bri_wave = wrap(bri_wave);


  // === BLEND =================================================================
  float blendWave = bri_wave*u_blend;
  blendWave = blendWave>0.5 ? max(blendWave, u_blend) : min(blendWave, u_blend);
  vec3 blend_col = mix(col, wallCol, blendWave);


  // === HUE WAVES =============================================================
  vec3 hsvCol = rgb2hsv(blend_col.rgb);
  hsvCol.x += 0.5 * u_colorSpeed * u_colorSpeed * TIME;
  hsvCol.x += u_colorWave * u_colorWave * (sin(st.x-0.5) + cos(st.y-1.));


  gl_FragColor = vec4(hsv2rgb(hsvCol), 1.);
  //gl_FragColor = vec4(vec3(blendWave), 1.);
}