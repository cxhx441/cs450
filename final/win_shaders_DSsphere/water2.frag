// make this 120 for the mac:
#version 330 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKambient, uKdiffuse, uKspecular;	 // coefficients of each type of lighting -- make sum to 1.0
uniform vec3    uColor;		 // object color
uniform vec3    uSpecularColor;	 // light color
uniform float   uShininess;	 // specular exponent
uniform float	uAlpha;		// alpha

// square-equation uniform variables -- these should be set every time Display( ) is called:

// in variables from the vertex shader and interpolated in the rasterizer:

in  vec3  gNormal;		   // normal vector
in  vec3  gPointToLight;		   // vector from point to light
in  vec3  gPointToEye;		   // vector from point to eye
in  vec2  gST;		   // (s,t) texture coordinates


void
main( )
{
	float s = gST.s;
	float t = gST.t;

	vec3 myColor = uColor;
	// apply the per-fragmewnt lighting to myColor:

	vec3 Normal = normalize(gNormal);
	vec3 Light  = normalize(gPointToLight);
	vec3 Eye    = normalize(gPointToEye);

	vec3 ambient = uKambient * myColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKdiffuse * dd * myColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKspecular * ss * uSpecularColor;
	gl_FragColor = vec4( (ambient + diffuse + specular)*uAlpha,  1. );
}

