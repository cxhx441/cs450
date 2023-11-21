#version 330 compatibility

uniform float uX0, uY0, uD;  // from your program
in vec2 vXY; // from the vertex shader, interpolated through the rasterizer
in vec3 aColor;

void
main( )
{
	float x = vXY.x; // the s coordinate of where this fragment is
	float y = vXY.y; // the t coordinate of where this fragment is
	vec3 myColor = aColor; // default color

	//if( uS0 - uD/2. <= s && s <= uS0 + uD/2. && uT0 - uD/2. <= t && t <= uT0 + uD/2. )
	if( x >= uX0 )
	{
		myColor = vec3( 1., 0., 0. ); // new pattern color
	}

	gl_FragColor = vec4(myColor, 1);
	//gl_FragColor = << myColor with lighting applied >>

}