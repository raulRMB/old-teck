void set_vector_element(inout float2 vec, int idx, float val) {
  vec = (idx.xx == int2(0, 1)) ? val.xx : vec;
}

void set_vector_element_1(inout int3 vec, int idx, int val) {
  vec = (idx.xxx == int3(0, 1, 2)) ? val.xxx : vec;
}

void set_vector_element_2(inout uint4 vec, int idx, uint val) {
  vec = (idx.xxxx == int4(0, 1, 2, 3)) ? val.xxxx : vec;
}

void set_vector_element_3(inout bool2 vec, int idx, bool val) {
  vec = (idx.xx == int2(0, 1)) ? val.xx : vec;
}

[numthreads(1, 1, 1)]
void main() {
  float2 v2f = float2(0.0f, 0.0f);
  float2 v2f_2 = float2(0.0f, 0.0f);
  int3 v3i = int3(0, 0, 0);
  int3 v3i_2 = int3(0, 0, 0);
  uint4 v4u = uint4(0u, 0u, 0u, 0u);
  uint4 v4u_2 = uint4(0u, 0u, 0u, 0u);
  bool2 v2b = bool2(false, false);
  bool2 v2b_2 = bool2(false, false);
  {
    for(int i = 0; (i < 2); i = (i + 1)) {
      set_vector_element(v2f, i, 1.0f);
      set_vector_element_1(v3i, i, 1);
      set_vector_element_2(v4u, i, 1u);
      set_vector_element_3(v2b, i, true);
      set_vector_element(v2f_2, i, 1.0f);
      set_vector_element_1(v3i_2, i, 1);
      set_vector_element_2(v4u_2, i, 1u);
      set_vector_element_3(v2b_2, i, true);
    }
  }
  return;
}
