#version 330
precision highp float;uniform vec2 resolution;uniform vec2 mouse;uniform vec2 upperLeft;uniform vec2 lowerRight;uniform int  max_iter;float d(vec2 a, vec2 b) {return sqrt(pow(a.x - b.x, 2.0) + pow(a.y - b.y, 2.0));}
float mandelbrot(vec2 c) {vec2 z = vec2(0.0, 0.0);int i = 0;for(; i<= max_iter; ++i) {z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;if(d(z, vec2(0.0,0.0)) > 2.0)return float(i)/float(max_iter);}return 0;}
vec3 color(float k) {float r = 9.0 * (1.0 - k) * k * k * k;float g = 15.0 * (1.0 - k) * (1.0 - k) * k * k;float b = 8.5 * (1.0 - k) * (1.0 - k) * (1.0 - k) * k;return vec3(r, g, b);}
void main() {vec2 c = vec2(upperLeft.x + (lowerRight.x - upperLeft.x) * gl_FragCoord.x / resolution.x,upperLeft.y + (lowerRight.y - upperLeft.y) * gl_FragCoord.y / resolution.y);c.x *= resolution.x / resolution.y;float m = mandelbrot(c);vec2 mp = mouse/resolution;gl_FragColor = vec4(color(m), 1.0);}
