[numthreads(1, 1, 1)]
void unused_entry_point() {
  return;
}

static float3 u = (1.0f).xxx;

void f() {
  bool3 v = bool3(u);
}
