

__constant float step = 0.02;
__constant float BOUND = 80;
__constant float GRAV_CONST = 10;

typedef struct node
{
  float4 posW;
  float4 velO;
};

node self(node x)
{
  x.posW.xyz += posW.xyz * step;

  if (length(x.posW.xyz) > BOUND) {
    x.velO.xyz *= -0.05;
    x.posW.xyz = normalize(x.posW.xyz) * BOUND;
  }

  return x;
}

node inter(node self, node other)
{
  float4 dist = self.posW - other.posW;
  dist.w = 0;

  float tmp = step * step * other.posW.w * normalize(dist);
  self.velO.xyz -= tmp / (dot(dist, dist) + GRAV_CONST);

  return self;
}

__kernel
void direct(__global node *read, __global node *write)
{
  uint i, j;
  size_t index = get_global_id(0);
  node n = read[index];

  n = self(n);

  for (i = 0; i < get_global_size(0); i++) {
    n = inter(n, read[n]);
  }

  write[index] = n;
}