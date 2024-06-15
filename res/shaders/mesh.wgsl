struct MVP {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    projection: mat4x4<f32>
}

struct Vertex {
    @location(0) position: vec2f,
    @location(1) color: vec3f
}

struct VertexOut {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f
}

@group(0) @binding(0) var<uniform> mvp: MVP;

@vertex
fn vs_main(vert : Vertex) -> VertexOut {
    var out: VertexOut;
    out.position = mvp.projection * mvp.view * mvp.model * vec4f(vert.position, 0.0, 1.0);
//    out.position = vec4f(vert.position, 0.0, 1.0);
    out.color = vert.color;
    return out;
}

@fragment
fn fs_main(vert : VertexOut) -> @location(0) vec4f {
    return vec4f(1.,1.,1., 1.0);
//    return vec4f(vert.color, 1.0);
}
