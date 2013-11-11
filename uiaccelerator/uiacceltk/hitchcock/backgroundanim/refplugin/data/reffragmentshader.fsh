precision mediump float;

uniform vec3 blob1;
uniform vec3 blob2;
uniform vec3 blob3;


uniform float offset;
uniform float threshold;
varying vec2  texCoord;

void main()
    {
    vec2  fp = texCoord;//vec2(gl_FragCoord[0], gl_FragCoord[1]);

    vec2  p1 = vec2(blob1[0], blob1[1]);
    float d1 = distance(fp,p1);
    float acc = blob1[2]/(d1*d1);

    vec2  p2 = vec2(blob2[0], blob2[1]);
    float d2 = distance(fp,p2);
    acc += blob2[2]/(d2*d2);

    vec2  p3 = vec2(blob3[0], blob3[1]);
    float d3 = distance(fp,p3);
    acc += blob3[2]/(d3*d3);

    float col = 1.0-((acc/threshold)-offset);
    if (col < 0.35)
       {
       col = 0.35;
       }

    gl_FragColor = vec4(col*0.6,col*0.72,col*0.77,1.0);
    }
