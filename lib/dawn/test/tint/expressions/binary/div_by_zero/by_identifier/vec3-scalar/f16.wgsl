enable f16;
@compute @workgroup_size(1)
fn f() {
    var a = vec3<f16>(1.h, 2.h, 3.h);
    var b = 0.h;
    let r : vec3<f16> = a / b;
}
