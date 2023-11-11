#line 1000
vec2 boxIntersector( vec3 ro, vec3 rd, vec3 box_hw, out vec3 box_n) 
{
	ro -= box_hw;
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*box_hw;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( t1.x, max(t1.y, t1.z) );
    float tF = min( t2.x, min(t2.y, t2.z) );
    if( tN>tF || tF<0.0) return vec2(-1.0); // no intersection

    box_n = (tN>0.0) ? step(vec3(tN),t1): step(t2,vec3(tF));
    box_n = -sign(rd)*box_n;

    return vec2(tN, tF);
}

float planeIntersector( in vec2 ro, in vec2 rd, in vec3 p )
{
    return -(dot(ro, p.xy)+p.z)/dot(rd, p.xy);
}

//https://www.shadertoy.com/view/ttlBWf
float boxOcclusion( in vec3 pos, in vec3 nor, in mat4 txx, in mat4 txi, in vec3 rad ) 
{
	vec3 p = (txx*vec4(pos,1.0)).xyz;
	vec3 n = (txx*vec4(nor,0.0)).xyz;
    
    // Orient the hexagon based on p
    vec3 f = rad * sign(p);
    
    // Make sure the hexagon is always convex
    vec3 s = sign(rad - abs(p));
    
    // 6 verts
    vec3 v0 = normalize( vec3( 1.0, 1.0,-1.0)*f - p);
    vec3 v1 = normalize( vec3( 1.0, s.x, s.x)*f - p);
    vec3 v2 = normalize( vec3( 1.0,-1.0, 1.0)*f - p);
    vec3 v3 = normalize( vec3( s.z, s.z, 1.0)*f - p);
    vec3 v4 = normalize( vec3(-1.0, 1.0, 1.0)*f - p);
    vec3 v5 = normalize( vec3( s.y, 1.0, s.y)*f - p);
    
    // 6 edges
    return abs( dot( n, normalize( cross(v0,v1)) ) * acos( dot(v0,v1) ) +
    	    	dot( n, normalize( cross(v1,v2)) ) * acos( dot(v1,v2) ) +
    	    	dot( n, normalize( cross(v2,v3)) ) * acos( dot(v2,v3) ) +
    	    	dot( n, normalize( cross(v3,v4)) ) * acos( dot(v3,v4) ) +
    	    	dot( n, normalize( cross(v4,v5)) ) * acos( dot(v4,v5) ) +
    	    	dot( n, normalize( cross(v5,v0)) ) * acos( dot(v5,v0) ))
            	/ 6.2831;
}

//Optimized for axis-aligned unit box
float boxOcclusionOpt( in vec3 pos, in vec3 n, in vec3 box_pos, in vec3 rad) 
{
	vec3 p = pos-box_pos;

    // Orient the hexagon based on p
    vec3 f = rad * sign(p);
    
    // Make sure the hexagon is always convex
    vec3 s = sign(rad - abs(p));
    
    // 6 verts
    vec3 v0 = normalize( vec3( 1.0, 1.0,-1.0)*f - p);
    vec3 v1 = normalize( vec3( 1.0, s.x, s.x)*f - p);
    vec3 v2 = normalize( vec3( 1.0,-1.0, 1.0)*f - p);
    vec3 v3 = normalize( vec3( s.z, s.z, 1.0)*f - p);
    vec3 v4 = normalize( vec3(-1.0, 1.0, 1.0)*f - p);
    vec3 v5 = normalize( vec3( s.y, 1.0, s.y)*f - p);
    
    // 6 edges
    return abs( dot( n, normalize( cross(v0,v1)) ) * acos( dot(v0,v1) ) +
    	    	dot( n, normalize( cross(v1,v2)) ) * acos( dot(v1,v2) ) +
    	    	dot( n, normalize( cross(v2,v3)) ) * acos( dot(v2,v3) ) +
    	    	dot( n, normalize( cross(v3,v4)) ) * acos( dot(v3,v4) ) +
    	    	dot( n, normalize( cross(v4,v5)) ) * acos( dot(v4,v5) ) +
    	    	dot( n, normalize( cross(v5,v0)) ) * acos( dot(v5,v0) ))
            	/ 6.2831;
}

//https://www.shadertoy.com/view/WslGz4
float boxSoftShadow( in vec3 ro, in vec3 rd, in vec3 box_hw, in vec3 box_pos, in float sk ) 
{
	ro -= box_pos+box_hw;
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*box_hw;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

    float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	
    // fake soft shadow
    if( tF<0.0) return 1.0;
    float sh = clamp(0.3*sk*(tN-tF)/tN,0.0,1.0);
    return sh;
    //return sh*sh*(3.0-2.0*sh);
}  

