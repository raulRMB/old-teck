// flags:  --hlsl_shader_model 62
enable f16;
var<private> t : f16;
fn m() -> vec2<f16> {
    t = 1.0h;
    return vec2<f16>(t);
}
fn f() {
    var v : vec2<f32> = vec2<f32>(m());
}