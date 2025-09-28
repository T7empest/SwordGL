#version 460

layout(location = 0) in  vec4 v_color;   // nevyužito (ponecháno pro kompatibilitu)
layout(location = 0) out vec4 frag_color;

layout(std140, set = 3, binding = 0) uniform UniformBlock {
    vec2  resolution;  // px
    float time;        // s
    float _pad;
};

mat2 rot(float a) { float c = cos(a), s = sin(a); return mat2(c,-s, s, c); }

float orb(vec3 p) {
    float t = time * 2.0;
    vec3 c = vec3(
    sin(sin(t*0.2)+t*0.4) * 6.0,
    1.0 + sin(sin(t*0.5)+t*0.2) * 4.0,
    12.0 + time + cos(t*0.3) * 8.0
    );
    return length(p - c);
}

void main() {
    // UV nezávislé na rozlišení ~[-1,1] na obou osách
    float smin = min(resolution.x, resolution.y);
    vec2  uv   = (gl_FragCoord.xy - 0.5 * resolution) / smin;

    // jemný pohyb „kamery“
    float t = time;
    uv += vec2(cos(t*0.10)*0.25, cos(t*0.30)*0.10);

    // méně kroků + o něco větší krok -> ostřejší struktura
    const int   STEPS   = 96;
    const float FAR     = 48.0;

    float d    = 0.0;   // nasbíraná vzdálenost
    float acc  = 0.0;   // akumulovaná „jasnost“
    float emin = 1e9;   // min. vzdálenost k orbu

    for (int i = 0; i < STEPS; ++i) {
        // jednoduchý paprsek: ro=(0,0,0), rd≈normalize(vec3(uv,1)) ~ zde parallax:
        vec3 p = vec3(uv * d, d + t);

        // vzdálenost k orbu (lehce zmenšený poloměr)
        float e = max(orb(p) - 0.12, 0.0);
        emin = min(emin, e);

        // twist podle hloubky/času
        p.xy = rot(0.10 * t + p.z * 0.12) * p.xy;

        // základ „sálových“ stěn a pseudo-turbulence (levné, ale výrazné)
        float s = 4.0 - abs(p.y);

        // kratší turbulence smyčka -> méně blur, víc „kouře“
        float A = 1.0;
        for (int k = 0; k < 4; ++k) {
            p += cos(0.65 * t + p.yzx) * 0.18;
            s -= abs(dot(sin(0.10 * t + p * A), vec3(0.6))) / A;
            A *= 1.9;
        }

        // ostřejší krokování (větší minimální krok + menší „měkké“ minimum)
        float stepLen = min(0.04 + 0.22 * abs(s), max(0.45 * e, 0.008));
        d += stepLen;

        // akumulace jasu — zvýrazněná blízkost k „sazeninám“ kouře
        acc += 1.0 / (stepLen + e * 2.6);

        if (d > FAR) break;
    }

    // --- výstup: čistě grayscale, vyšší kontrast ---
    // základní intenzita z akumulace
    float lum = acc / 11.0;

    // lokální kontrast (S-křivka) + lehké zvýraznění jádra kouře
    lum = smoothstep(0.10, 0.95, lum);
    lum += exp(-emin * 0.75) * 0.15;         // jemná „mlžná“ záře okolo orbu
    lum = clamp(lum, 0.0, 1.0);

    // „ostřejší“ gamma (méně rozmazané)
    lum = pow(lum, 1.0/1.6);

    frag_color = vec4(vec3(lum), 1.0);
}
