RWByteAddressBuffer prevent_dce : register(u0, space2);

void atan_331e6d() {
  float3 arg_0 = (1.0f).xxx;
  float3 res = atan(arg_0);
  prevent_dce.Store3(0u, asuint(res));
}

struct tint_symbol {
  float4 value : SV_Position;
};

float4 vertex_main_inner() {
  atan_331e6d();
  return (0.0f).xxxx;
}

tint_symbol vertex_main() {
  float4 inner_result = vertex_main_inner();
  tint_symbol wrapper_result = (tint_symbol)0;
  wrapper_result.value = inner_result;
  return wrapper_result;
}

void fragment_main() {
  atan_331e6d();
  return;
}

[numthreads(1, 1, 1)]
void compute_main() {
  atan_331e6d();
  return;
}
