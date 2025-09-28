#version 450
layout(location=0) in  vec2 vUV;
layout(location=1) in  flat vec4 vColor;
layout(location=2) in  flat uint vSeed;
layout(location=3) in  flat float vAge01;
layout(location=0) out vec4 outColor;

layout(std140, set=3, binding=0) uniform UniformBlock {
    vec2  resolution;
    float time;
    float _pad;
} U;

float sat(float x){ return clamp(x,0.0,1.0); }
float hash11(float x){ x=fract(x*0.1031); x*=x+33.33; x*=x+x; return fract(x); }
float hash12(vec2 p){ p=fract(p*vec2(123.34,456.21)); p+=dot(p,p+34.56); return fract(p.x*p.y); }
float noise(vec2 p){
    vec2 i=floor(p), f=fract(p);
    float a=hash12(i), b=hash12(i+vec2(1,0));
    float c=hash12(i+vec2(0,1)), d=hash12(i+vec2(1,1));
    vec2 u=f*f*(3.0-2.0*f);
    return mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
}
mat2 rot(float a){ float c=cos(a), s=sin(a); return mat2(c,-s,s,c); }

void main(){
    float t = U.time;

    // ---- základní UV [-1,1]
    vec2 p = vUV * 2.0 - 1.0;

    // ---- per-particle náhoda
    float s0 = hash11(float(vSeed)*1.17);
    float s1 = hash11(float(vSeed)*3.71);
    float s2 = hash11(float(vSeed)*7.91);

    // ---- NEpravidelná silueta (elipsa + warp + šum) -> z toho počítáme alpha
    vec2 pe = rot((s0*2.0 - 1.0)*3.14159) * p;

    // jemná elipsa (náhodné osy)
    vec2 eScale = vec2(mix(0.80, 1.25, s1), mix(0.80, 1.25, s2));
    pe /= eScale;

    // okrajový warp (mírný, isotropní)
    float eFrq = mix(2.0, 5.0, s1);
    float eAmp = mix(0.08, 0.18, s2);
    vec2  en   = vec2(noise(pe*eFrq + t*0.35),
    noise(pe*eFrq*1.21 - t*0.27));
    pe += (en - 0.5) * eAmp;

    // šum pro „zubatost“ hran – moduluje signovanou vzdálenost
    float jag = (noise(pe*eFrq*0.8 + t*0.17) - 0.5) * 0.12;

    float rE      = length(pe);
    float dShape  = 1.0 - rE;          // >0 uvnitř
    dShape       -= jag;               // nepravidelný obrys
    float aa      = fwidth(dShape);
    float edge    = smoothstep(0.0, aa * 9.9, dShape); // VÍC "blurred" okraj

    // ---- vnitřní kouř (grayscale), používá podobné souřadnice
    vec2  qp   = pe; // můžeš přidat jemnější warp, ale siluetu to už nemění
    float r    = length(qp);
    float core = 0.78 - 0.30 * smoothstep(0.0, 0.72, r);
    float rim  = 0.32 * smoothstep(0.48, 0.96, r);
    float grain= noise(qp*5.0 + t*0.2 + s0*10.0);
    float dens = (core + rim) * mix(0.9, 1.12, grain);

    // ---- oranžový žár (maskovaný tvarem + středem)
    float age       = sat(vAge01);
    float ember     = (age > 0.95) ? 1.0 : 0.0;      // tvoje logika konce života
    float radial    = exp(-12.0 * rE * rE);        // střed podle tvaru
    float emberMask = edge * radial;               // nikdy mimo siluetu
    vec3  emberColor= vec3(1.0, 0.45, 0.10);

    // ---- finále (straight alpha)
    vec3  col   = vec3(dens) + emberColor * (0.8 * ember * emberMask);
    float alpha = vColor.a * edge * mix(0.85, 1.0, grain);
    alpha = sat(alpha + 0.8 * ember * emberMask);

    outColor = vec4(col, alpha);
}
