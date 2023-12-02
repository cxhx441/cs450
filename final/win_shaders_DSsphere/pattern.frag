// make this 120 for the mac:
#version 330 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKambient, uKdiffuse, uKspecular;	 // coefficients of each type of lighting -- make sum to 1.0
uniform vec3    uColor;		 // object color
uniform vec3    uSpecularColor;	 // light color
uniform float   uShininess;	 // specular exponent
uniform float	uAlpha; // color alpha
uniform vec3	uLightColor;

// square-equation uniform variables -- these should be set every time Display( ) is called:

uniform float   uSc, uTc, uRt, uRs;

// in variables from the vertex shader and interpolated in the rasterizer:

in  vec3  vNormal;		   // normal vector
in  vec3  vPointToLight;		   // vector from point to light
in  vec3  vPointToEye;		   // vector from point to eye
in  vec2  vST;		   // (s,t) texture coordinates


void
main( )
{
	float s = vST.s;
	float t = vST.t;

	// determine the color using the square-boundary equations:

	vec3 myColor = uColor;
	// apply the per-fragmewnt lighting to myColor:

	vec3 Normal = normalize(vNormal);
	vec3 Light  = normalize(vPointToLight);
	vec3 Eye    = normalize(vPointToEye);

	vec3 ambient = uKambient * myColor *uLightColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKdiffuse * dd * myColor * uLightColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKspecular * ss * uSpecularColor * uLightColor;
	gl_FragColor = vec4( (ambient + diffuse + specular)*uAlpha,  1. );
}

