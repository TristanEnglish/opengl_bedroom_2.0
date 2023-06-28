// Toon Shading
// Fragment shader
#version 120

varying vec3 Light;
varying vec3 Normal;
uniform sampler2D tex;

void main()
{
    vec3 color;
    float f;

    float intensity = dot(normalize(Normal), normalize(Light));

    if (intensity > 0.95) {
        f = 1.0; // highest level of shading - white
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }
    else if (intensity > 0.7) {
        f = 0.8; // medium level of shading - dark color
    }
    else if (intensity > 0.5) {
        f = 0.6; // medium level of shading - dark color
    }
    else if (intensity > 0.25) {
        f = 0.4; // low level of shading - darker color
    }
    else {
        f = 0.1; // lowest level of shading - darkest color
    }

    vec4 texcolor = texture2D(tex,gl_TexCoord[0].xy);

    gl_FragColor = vec4(f * texcolor.r, f * texcolor.g, f * texcolor.b, 1.0) * (vec4(f * gl_Color.r, f * gl_Color.g, f * gl_Color.b, 1.0) + 1.0);

}

