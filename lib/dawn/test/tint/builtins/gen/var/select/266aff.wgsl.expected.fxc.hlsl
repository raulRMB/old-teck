RWByteAddressBuffer prevent_dce : register(u0, space2);

void select_266aff() {
  float2 arg_0 = (1.0f).xx;
  float2 arg_1 = (1.0f).xx;
  bool2 arg_2 = (true).xx;
  float2 res = (arg_2 ? arg_1 : arg_0);
  prevent_dce.Store2(0u, asuint(res));
}

struct tint_symbol {
  float4 value : SV_Position;
};

float4 vertex_main_inner() {
  select_266aff();
  return (0.0f).xxxx;
}

tint_symbol vertex_main() {
  float4 inner_result = vertex_main_inner();
  tint_symbol wrapper_result = (tint_symbol)0;
  wrapper_result.value = inner_result;
  return wrapper_result;
}

void fragment_main() {
  select_266aff();
  return;
}

[numthreads(1, 1, 1)]
void compute_main() {
  select_266aff();
  return;
}
