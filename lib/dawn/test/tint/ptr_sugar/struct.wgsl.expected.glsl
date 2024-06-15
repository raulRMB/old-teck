#version 310 es

struct S {
  int x;
};

void deref() {
  S a = S(0);
  int b = a.x;
  a.x = 42;
}

void no_deref() {
  S a = S(0);
  int b = a.x;
  a.x = 42;
}

void tint_symbol() {
  deref();
  no_deref();
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  tint_symbol();
  return;
}
