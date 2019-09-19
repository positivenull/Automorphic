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
// === CLOUDS ================================================================
// ===========================================================================
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*1.93;
mat2 rot(float a){
    float c = cos(a);
    float s = sin(a);
    return mat2(c, s, -s, c);
}
float mag2(vec2 p){
    return dot(p, p);
}
float linstep(float mn, float mx, float x){
    return clamp((x - mn)/(mx - mn), 0., 1.);
}
float prm1 = 0.;
vec2 bsMo = vec2(0);
vec2 disp(float t){
    return vec2(sin(t*0.22)*1., cos(t*0.175)*1.)*2.;
}
vec2 map(vec3 p) {
    vec3 p2 = p;
    p2.xy -= disp(p.z).xy;
    p.xy *= rot(sin(p.z+TIME)*(0.1 + prm1*0.05) + TIME*0.09);
    float cl = mag2(p2.xy);
    float d = 0.;
    p *= .61;
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1*0.2;
    for(int i = 0; i < 5; i++)
    {
		p += sin(p.zxy*0.75*trk + TIME*trk*.8)*dspAmp;
        d -= abs(dot(cos(p), sin(p.yzx))*z);
        z *= 0.57;
        trk *= 1.4;
        p = p*m3;
    }
    d = abs(d + prm1*3.)+ prm1*.3 - 2.5 + bsMo.y;
    return vec2(d + cl*.2 + 0.25, cl);
}

vec4 render(vec3 ro, vec3 rd, float time) {
	vec4 rez = vec4(0);
    const float ldst = 8.;
	vec3 lpos = vec3(disp(time + ldst)*0.5, time + ldst);
	float t = 1.5;
	float fogT = 0.;
	for(int i=0; i<130; i++)
	{
		if(rez.a > 0.99)break;

		vec3 pos = ro + t*rd;
        vec2 mpv = map(pos);
		float den = clamp(mpv.x-0.3,0.,1.)*1.12;
		float dn = clamp((mpv.x + 2.),0.,3.);
        
		vec4 col = vec4(0);
        if (mpv.x > 0.6)
        {
        
            col = vec4(sin(vec3(5.,0.4,0.2) + mpv.y*0.1 +sin(pos.z*0.4)*0.5 + 1.8)*0.5 + 0.5,0.08);
            col *= den*den*den;
			col.rgb *= linstep(4.,-2.5, mpv.x)*2.3;
            float dif =  clamp((den - map(pos+.8).x)/9., 0.001, 1. );
            dif += clamp((den - map(pos+.35).x)/2.5, 0.001, 1. );
            col.xyz *= den*(vec3(0.005,.045,.075) + 1.5*vec3(0.033,0.07,0.03)*dif);
        }
		
		float fogC = exp(t*0.2 - 2.2);
		col.rgba += vec4(0.06,0.11,0.11, 0.1)*clamp(fogC-fogT, 0., 1.);
		fogT = fogC;
		rez = rez + col*(1. - rez.a);
		t += clamp(0.5 - dn*dn*.05, 0.09, 0.3);
	}
	return clamp(rez, 0.0, 1.0);
}

float getsat(vec3 c) {
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//from my "Will it blend" shader (https://www.shadertoy.com/view/lsdGzN)
vec3 iLerp(vec3 a, vec3 b, float x) {
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    float lgt = dot(vec3(1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

vec3 mainClouds(){
    vec2 q = gl_FragCoord.xy/RENDERSIZE.xy;
    vec2 p = (gl_FragCoord.xy - 0.5*RENDERSIZE.xy)/RENDERSIZE.y;
	p = p*5; // zoom out
    // bsMo = (iMouse.xy - 0.5*RENDERSIZE.xy)/RENDERSIZE.y;
    bsMo = vec2(0, 0);
    
    float time = -TIME*3.;
    vec3 ro = vec3(0,0,time);
    
    ro += vec3(sin(TIME)*0.,sin(TIME*1.)*0.,0);
        
    float dspAmp = .85;
    ro.xy += disp(ro.z)*dspAmp;
    float tgtDst = 3.5;
    
    vec3 target = normalize(ro - vec3(disp(time + tgtDst)*dspAmp, time + tgtDst));
    ro.x -= bsMo.x*2.;
    vec3 rightdir = normalize(cross(target, vec3(0,1,0)));
    vec3 updir = normalize(cross(rightdir, target));
    rightdir = normalize(cross(updir, target));
	vec3 rd = normalize((p.x*rightdir + p.y*updir)*1. - target);
    rd.xy *= rot(-disp(time + 3.5).x*0.2 + bsMo.x);
    prm1 = smoothstep(-0.4, 0.4,sin(TIME*0.3));
	vec4 scn = render(ro, rd, time);
		
    vec3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1.-prm1,0.05,1.));
    
    col = pow(col, vec3(.55,0.65,0.6))*vec3(1.,.97,.9);

    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12)*0.7+0.3; //Vign

    return col.rgb;
}


// ===========================================================================
// === ALIEN LATICE ==========================================================
// ===========================================================================

#define PI 3.14159265358979
#define TAU 6.28318530718

// 2D vector rotation. Very handy, and used virtually all the u_time, especially when pivoting the camera.
vec2 rot(vec2 p, float a) {
	float c = cos(a); float s = sin(a);
	return vec2(c*p.x - s*p.y, s*p.x + c*p.y);
}

// Greyscale. Also very handy.
float getGrey(vec3 p){ return p.x*0.299 + p.y*0.587 + p.z*0.114; }

// Hash to return a scalar value from a 3D vector.
float hash31(vec3 p){ return fract(sin(dot(p, vec3(127.1,311.7, 74.7)))*43758.5453); }

// Standard hash algorithm that you'll see all over the place.
vec3 hash33(vec3 p) { 
    float n = sin(dot(p, vec3(7, 157, 113)));
    return fract(vec3(2097152, 262144, 32768)*n)*2.-1.;  
}

// I wrote a lot of this off the top of my head. Partition 3D space into tetrahedrons, throw in some random vectors at the vertices,
// combine them with the vertice points, then some falloff to produce a value. It's simplex noise, for all intents and purposes,
// but I called this "tetraNoise," because I don't want anyone to mistake this for a comp-science-quality simplex noise algorithm. 
// Amazingly, it seems to work, though, so there's hope for me yet.
// Credits: Ken Perlin, Brian Sharpe (does great work), IQ, etc.
float tetraNoise( vec3 p ){
    // First corner, with skewing, unskewing, and so forth.
    vec3 i  = floor(p + dot(p, vec3(0.333333)) );
    vec3 p0 = p - i + dot(i, vec3(0.166666));

    // Kind of semi-branchless way to determine which side of a couple of planes we're on.
    vec3 g = step(p0.yzx, p0);

    vec3 i1 = min( g, 1.0 - g.zxy );
    vec3 i2 = max( g, 1.0 - g.zxy );

    // Other corners, based on the geometry of a tetrahedron.
    vec3 p1 = p0 - i1 + 0.166666;
    vec3 p2 = p0 - i2 + 0.333333;
    vec3 p3 = p0 - 0.5;     

    // Vector to store the squared distances from the vertex positions to our point in space, arranged to fall off
    // to zero.
    vec4 v = max(0.5 - vec4(dot(p0,p0), dot(p1,p1), dot(p2,p2), dot(p3,p3)), 0.0)*2.;

    // Cheaper (considerably), but less visually appealing.
    //vec4 d = vec4(hash31(i), hash31(i + i1), hash31(i + i2), hash31(i + 1.))*2.-1.; // Weighted value, for each vertice.
    //return dot(d*0.5, v*v*v)+0.5;

    // Dotting each vertice with a gradient produced at each vertice.
    vec4 d = vec4(dot(p0, hash33(i)), dot(p1, hash33(i + i1)), dot(p2, hash33(i + i2)), dot(p3, hash33(i + 1.))); // Weighted value, for each vertice.

    // It took me ages to decide on the figure "1.732" (sqrt(3.)). The logic made sense at the time, and it seems to work... but it could change.
    // There is logic behind cubing (a lot of people use a power of 4, which is not for me), but it'd take longer to explain than I have here,
    // so for now, just except that it's what you do. :)
    return dot(d, v*v*v)*1.732 + 0.5; 
   
}

// Used to color the coral. Not that interesting.
float sinCol(vec3 p){
    return (sin(p.x)*sin(p.y)*cos(p.z)*0.66+sin(p.x*2.)*sin(p.y*2.)*cos(p.z*2.)*0.34)*0.5+0.5;  // Range: [0, 1]
}

// The function used for bump mapping. A bit of 3D noise run through a sinusoid for a bit of variance. Nothing exciting.
float surfFunc(vec3 p){
	float n = tetraNoise(p*8.);
	n = 0.5-0.5*cos((n)*TAU*2.);
	return (sqrt(n)+n)*0.5;
}

// Technically, this is the most important function in any raymarching application. It's also, arguably, the one that gets called the most u_times,
// so the functions you use should be cheap, but have maximum impact. It might look a little complicated, but in essence, it's just one big 
// sinusoidal structure, with some moving sinusoidal bumps placed on it. On a GPU, sinusoidal functions tend to be cheap.
float mapAl(vec3 p) {
    float t  = TIME*0.375; // time component used to move the bumps around. Set it to zero to keep the structure rigid.
	float k  = cos(p.z * 1.95 + t)*sin(p.y * 2.5 + t)*cos(p.x * 3.05 + t); // Layer of sinusoidal bumps.
	float k2 = sin(p.z * 6.03 + t*2.)*cos(p.y * 4.98 + t*2.)*sin(p.x * 3.93 + t*2.); // Second layer of bumps, at roughly double the frequency.
	
	float s = cos(p.x) + cos(p.y) + sin(p.z); // The main structure. Just a big sinusoidal cavern, of sorts.
	s = (s*0.25 + (s*s/2.)*0.75); // Mutating the main structure. You don't have to, but it adds a bit of variation.
	
	// Adding the two layers of moving sinusoidal bumps to the structure. Take away the bumps, and you're left with a pretty lacklustre
	// looking structure. It'd be nice to add a fancy noise algorithm, but that would slay the framerate.
	return s + (k*0.66 + k2*0.34)*0.5; 
}

// Based on original code by IQ, and if memory serves me correctly, one of the coders from Razor 1911.
float ambientOcclusion( in vec3 p, in vec3 n, in float k ) {
    const float AO_SAMPLES = 6.0;
    float d = 0.0;
    float oc = 0.0;
    
    for(float i=1.; i<=AO_SAMPLES; i++){
		d = k*i/AO_SAMPLES;
		oc += (d - mapAl(p+n*d))/exp(d);
    }
    oc /= AO_SAMPLES;
    
    return clamp(pow(1.-oc, k), 0.0, 1.0);
}

float softShadow(vec3 ro, vec3 rd, float start, float end, float k) {
    float shade = 1.0;
    const float maxIterationsShad = 24.;

    // The "start" value, or minimum, should be set to something more than the stop-threshold, so as to avoid a collision with 
    // the surface the ray is setting out from. It doesn't matter how many u_times I write shadow code, I always seem to forget this.
    // If adding shadows seems to make everything look dark, that tends to be the problem.
    float dist = start;
    float stepDist = end/maxIterationsShad;

    // Max shadow iterations - More iterations make nicer shadows, but slow things down. Obviously, the lowest 
    // number to give a decent shadow is the best one to choose. 
    for (float i=0.; i<maxIterationsShad; i++){
        // End, or maximum, should be set to the distance from the light to surface point. If you go beyond that
        // you may hit a surface not between the surface and the light.
        float h = mapAl(ro + rd*dist);
        shade = min(shade, k*h/dist);
        
        // What h combination you add to the distance depends on speed, accuracy, etc. To be honest, I find it impossible to find 
        // the perfect balance. Faster GPUs give you more options, because more shadow iterations always produce better results.
        // Anyway, here's some posibilities. Which one you use, depends on the situation:
        // +=h, +=clamp( h, 0.01, 0.25 ), +=min( h, 0.1 ), +=stepDist, +=min(h, stepDist*2.), etc.
        dist += min(h, stepDist*2.); // The best of both worlds... I think. 
        
        // Early exits from accumulative distance function calls tend to be a good thing.
        if (h<0.001 || dist > end) break; 
    }

    // I've added 0.3 to the final shade value, which lightens the shadow a bit. It's a preference thing. Really dark shadows look 
    // too brutal to me.
    return min(max(shade, 0.) + 0.3, 1.0); 
}

vec3 doBumpMap( in vec3 p, in vec3 nor, float bumpfactor ) {
    const float eps = 0.001;
	float ref = surfFunc( p );
	// Note: To save on calculations, we're stepping to just one side of the position "p," rather than both.
    vec3 grad = vec3( surfFunc(vec3(p.x+eps, p.y, p.z))-ref,
                      surfFunc(vec3(p.x, p.y+eps, p.z))-ref,
                      surfFunc(vec3(p.x, p.y, p.z+eps))-ref )/eps;
   
    // I tend to favor subtle bump mapping, so since this line has a subtle effect already, I have a bad habit of leaving it out. 
    // However, it should definitely be there, so if you do notice it missing in my other work, send me an email letting me know
    // what a lazy dumbass I am. :)              
    grad -= nor*dot(nor, grad);
    
    // Note the "-" sign, instead of the "+" one. It all depends on how the "grad" vector above is produced.
    // A lazy way to see if you've done it right is a good old-fashioned visual check.                     
    return normalize( nor - bumpfactor*grad );
}

// Obtain the surface normal at the surface point "p."
vec3 getNormal(in vec3 p) {
	const float eps = 0.001;
	return normalize(vec3(
		mapAl(vec3(p.x+eps,p.y,p.z))-mapAl(vec3(p.x-eps,p.y,p.z)),
		mapAl(vec3(p.x,p.y+eps,p.z))-mapAl(vec3(p.x,p.y-eps,p.z)),
		mapAl(vec3(p.x,p.y,p.z+eps))-mapAl(vec3(p.x,p.y,p.z-eps))
	));
    
    /*
    // If speed is an issue, here's a slightly-less-accurate, 4-tap version. If fact, visually speaking, it's virtually the same, so on a
    // lot of occasions, this is the one I'll use. However, if speed is really an issue, you could take away the "normalization" step, then 
    // divide by "eps," but I'll usually avoid doing that.
    float ref = eps(p);
	return normalize(vec3(
		eps(vec3(p.x+eps,p.y,p.z))-ref,
		eps(vec3(p.x,p.y+eps,p.z))-ref,
		eps(vec3(p.x,p.y,p.z+eps))-ref
	)); 
	*/
}

// Just a regular raymarching function, with some subtle ray distortion thrown in.
vec3 raymarchDistort(in vec3 ro, in vec3 rd) {
	const float FAR = 10.;
	
	float d = 1e4;
	
	float t = 0.0;
	float step = 0.5;
	
	for(int i=0; i<128; i++) {
		d = mapAl(ro + rd*t);
		
		if((d<0.01) || (t>FAR)) { break; }
		
		t += d*step;
		
		// This simple, little block is responsibe for the ray bending illusion that
		// you may have seen in various twisting spherical field demos, and so forth.
		// If you commented these out, you'd have a regular raymarching function.
		mat2 rt = rot(d*0.025);
		rd.xy *= rt;
		rd.yz *= rt;
		rd.zx *= rt;		
	}
	
	if(d<0.01) t += d;
	else t = FAR;
	
	return ro + rd*t;
}

// Cheap 2D water, and by cheap, I mean lazy. :) It's not an important part of the overall process, so
// doesn't need to be physics-grade convincing.
vec2 cheapWater2D( vec2 p ) {
    float l = length(p*2. + vec2(sin(TIME), cos(TIME)));
    return vec2(sin(l*4.), cos(l*4.))*0.1;
     
    //p = p*4. + TIME*0.25;
    //return cos( vec2( cos(p.x-p.y)*cos(p.y), sin(p.x+p.y)*sin(p.y) ) );
}


vec3 mainAlien( void ) {
	// Screen coordinates. This is about the easiest way to do it. In fact, it's my preferred method. 
	// The y range is [-1, 1], and the x-range is [-u_resolution.x/u_resolution.y, u_resolution.x/u_resolution.y].
	vec2 uv0  = (2.0*gl_FragCoord.xy - RENDERSIZE.xy)/RENDERSIZE.y;
	
	// This is a really cheesy way to peturb the screen a little to give an underwater effect. If you want 
	// to warp the viewing screen, this is where you'd do it. In the end, I decided against it, but left it
	// there for those who wish to see what it does.	
	//vec2 uv = uv0 + cheapWater2D(uv0)*0.1;
	
	// We're not bothering with screen perturbation effects (see above), so "uv" and "uv0" are the same thing.
	// I saved the original non-perturbed uv coordinates for a screen overlay effect below.
	vec2 uv = uv0;	
	
	// Speaking of warping effects, I've decided to use a curvy fish-eye lens. There are few ways to go about it,
	// but basically you create one by destorting, then spherizing, the z-component of your direction vector.
	// How much curve you give it is entirely up to you. This one is resonably mild.
	vec3 rd = normalize(vec3(uv, sqrt(1.-dot(uv,uv)*0.25)*0.75));
	//vec3 rd = normalize(vec3(uv, 0.75)); // This is the standard, non-warped direction vector.
	
	// We're not bothering with a proper camera setup, so we'll do our looking around by moving along the z-axis 
	// (going forward) and rotating the direction vector.
	rd.zy = rot(rd.zy, TIME * 0.25);
	rd.xz = rot(rd.xz, TIME * 0.125); 
    
    // The eye vector, viewing position, etc ("ro" for ray-origin, I'll asume). Moving forward with u_time.
	vec3 ro = vec3(0, 0, TIME);
    
    // Raymarch the scene, then return the surface position. The "raymarching" function is pretty stock standard,
    // except for a minor addition, which involves warping the direction vector ever so slightly. It's a common
    // raymarching trick that's definitely worth having in your repertoire. See the function for more detail.
    vec3 sp = raymarchDistort(ro, rd);	
	
	// The light position. It's been placed just a little above and behind the direction vector. It's also being
	// rotated to match the rotation of the direction vector (above) to stay behind it. 
	vec3 lp = vec3(0., 0.35, -0.25);
	lp.zy = rot(lp.zy, TIME * 0.25);
	lp.xz = rot(lp.xz, TIME * 0.125);
	lp += ro; 
	
	// The following block determines our normalized light-direction vector, and attains the distance between
	// the viewing postion (camera, eye, etc) and the surface point, "sp," of the scene object.
	vec3 ld = lp - sp;
	float dist = max(length(ld), 0.);	
	ld/=dist;
	
	// The distance determines the attenuation factor, which is just a fancy way to say, how much the light drops
	// off as we get further away from the light position. Hint: It usually gets darker. :)
	float atten = min(1./(dist*0.35+dist*dist*0.05), 1.);
	
	// Shadows, like always, slow things right down. Usually, I'd do some cost-cutting elsewhere
	// to make it work, but in this case, they didn't add much visually, so they didn't make the cut. :)
	float shad = 1.;//softShadow(sp, ld, 0.02, dist, 32.);
	
	
	// Determine the normal vector at the object's surface postion.
	vec3 n = getNormal(sp);
	
	// Apply bump mapping at the surface postion. Minor, but intricate, surface detail should almost always be 
	// applied via bump mapping. Putting it in the displacement (map, scene, etc) function can chew up a lot of cycles.
	
	// Without bump mapping, this scene would be even more lacklustre than it already is. Comment out the following 
	// line to see how much so. Actually, I just did, and it's not as bad as I thought. :)
	n = doBumpMap(sp, n, 0.025);	//min(2./sqrt(dist), 0.5)
	
	// Sometimes, I won't want the bump mapping to overshadow (sorry, couldn't resist) the ambient occlusion of the 
	// object itself, so I'll put it before the bump pass, but this is where you should usually put it.
	float ao = ambientOcclusion(sp, n, 2.);	
		
	// Diffuse and specular lighting factors.
	float dif = max(dot(n, ld), 0.);
    float spec = max( 0.0, dot( reflect(-ld, n), normalize(ro-sp)) ); //The object's specular value.
    spec = pow(spec, 32.); // Ramping up the specular value to the specular power for a bit of shininess.   
 
    // Coloring the object. 
    // Here's the color component. It's a mixture of science, and trial and error.
	float c = sinCol(sp*PI*0.5); // Range: [0, 1]
	vec3 objCol = vec3(1.-c*0.375, 0.5+c*0.5, c*0.75); // Blue, yellow, orange palette.
	objCol = objCol*sqrt(objCol*1.4142); // Enhancing the colors a bit.
	// This line is worth noting. To enhance the bump mapping, I like to shade the object with the bump function.
	// It's optional, but I prefer it. By the way, it's a wasted function call, but you could pass it into the
	// bump function, if you wanted to.
	objCol *= (surfFunc(sp)*0.6+0.4);
	
	// Lighting the object.
	// There are a million different ways to combine components to light an object. In gereral, though, you 
	// combine the object's material properties with the diffuse, specular and ambient properties 
	// (object, global, etc), then multiply them by the attenuation factor, ambient occlusion and shadow. 
	// It's definitely worth reading individual articles on this. 
	//
    // Normally, multiplying by the diffuse scalar value would do, but I wanted to brown it up a bit, because...
    // I can't remember why... Probably boredom.
	vec3 difCol = vec3(sqrt(dif), dif, dif*dif); 
	vec3 specCol =  vec3(0.5, 0.485, 0.46)*spec; // Specular color.
	vec3 ambCol = vec3(0.1); // Just a tiny bit of ambient light.
	// Normally, you would multiply the scalar attenunation value, and be done with it, but I've taken a bit of 
	// aritstic license and combined them to make the attenuation fade off diffenently for each color channel.
	// This gives the fade off a blue effect.
	vec3 attenCol = vec3(atten*atten, atten, sqrt(atten))*0.75 + atten*0.25;
	vec3 col = (objCol*(difCol + ambCol) + specCol)*attenCol*ao*shad;
	
    // Screen overlay. 
	// This mess gives a bit of a fuzzy horizontal line overlay. I made it up. as I went along, using a bit of 
	// random noise. Sadly, most things I code are coded this way.
	vec3 rnd = hash33(rd*vec3(64., 256., 256.)+TIME);
	if (mod(uv0.y*RENDERSIZE.x/2., 2.)>0.5 || rnd.x<-0.33)rnd = vec3(0.);
	else rnd = rnd*0.5+0.5;
	col = clamp(col + rnd*0.15, 0., 1.);
	// Comment out the above and use this, if you want to take away the cheesy, early 2000s demoscene static. :)
	//col = clamp(col, 0., 1.); 
	
	// Running a squarish vignette. Also made up on the spot. Comment out this block, if mid-to-late late 1990s 
	// borders aren't your thing. :)
	vec2 uvs = pow(abs(uv0)*vec2(RENDERSIZE.y/RENDERSIZE.x, 1.), vec2(8.));//
	float vignette = clamp(1.25-dot(uvs, uvs)*1.25, 0., 1.);//((1.-uvs.x)*uvs.x + (1.-uvs.y)*uvs.y);
	col *= vignette*0.9+0.1;
	
	// Finally, we're done. Set the shaded pixel color.
	return col;
}



// ===========================================================================
// === MAIN ==================================================================
// ===========================================================================

void main() {
    vec2 st = gl_FragCoord.xy / RENDERSIZE.xy - vec2(0.5);
    st.y += 0.03;
	st = st * (1 + vec2(u_gridSize - 4.)/4);

    vec3 colClouds = mainClouds();
    vec3 colAlien  = mainAlien();

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
    
    vec3 blend_col = mix(colAlien, colClouds, u_blend);
    
    // === HUE WAVES =============================================================
    vec3 hsvCol = rgb2hsv(blend_col.rgb);
    hsvCol.x += 0.5 * u_colorSpeed * u_colorSpeed * TIME;
    hsvCol.x += u_colorWave * u_colorWave * (sin(st.x-0.5) + cos(st.y-1.));

    gl_FragColor = vec4(hsv2rgb(hsvCol), 1.);
}