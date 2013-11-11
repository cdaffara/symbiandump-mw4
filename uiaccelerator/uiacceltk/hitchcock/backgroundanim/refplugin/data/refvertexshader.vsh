varying vec2  texCoord;
attribute vec4 vPosition;
uniform float aspect;

void main(void)
    {
    gl_Position = vPosition;
    texCoord = (vec2( gl_Position.x*aspect, - gl_Position.y ) + vec2( 1.0 ) ) / vec2( 2.0 );   
    }
