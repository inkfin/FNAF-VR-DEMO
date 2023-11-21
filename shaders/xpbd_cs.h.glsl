#line 2

struct XpbdParticle
{
   vec4 xpos;	//xpos.w == radius
   vec4 xprev;	//xprev.w == w (1.0/mass);
   vec4 vel;	//vel.w = mu (coefficent of friction)
   uvec4 flags; //flags.z = type, flags.w = rgba8 color
};

const uint XpbdMouseover = 1<<31;
const uint XpbdSelected = 1<<30;
const uint XpbdDragged = 1<<29;
const uint XpbdDefaultType = ~0;

float GetRadius(XpbdParticle p) {return p.xpos.w;}
void SetRadius(inout XpbdParticle p, float r) {p.xpos.w = r;}

bool IsDragged(XpbdParticle p) {return bool(p.flags[0] & XpbdDragged);}
bool IsDragged(uint flags) {return bool(flags & XpbdDragged);}

float GetW(XpbdParticle p) {if(IsDragged(p)) return 0.0; return p.xprev.w;}
void SetW(inout XpbdParticle p, float w) {p.xprev.w = w;}
bool IsDeleted(XpbdParticle p) {return p.xprev.w<0.0;}

float GetMu(XpbdParticle p) {return p.vel.w;}
void SetMu(inout XpbdParticle p, float w) {p.vel.w = w;}

uint GetType(XpbdParticle p)  {return p.flags.z;}
void SetType(inout XpbdParticle p, uint type) {p.flags.z = type;}

vec4 GetColor(XpbdParticle p) {return unpackUnorm4x8(p.flags.w);}
void SetColor(inout XpbdParticle p, vec4 color) {p.flags.w = packUnorm4x8(color);}

bool IsSelected(XpbdParticle p) {return bool(p.flags[0] & XpbdSelected);}
bool IsSelected(uint flags) {return bool(flags & XpbdSelected);}

struct DistanceConstraint
{
	ivec2 mEnd;
	float mL0;
	float mAlpha;

	float mLambda;
	uint mFlags;
	float mPadding0;
	float mPadding1;
};

bool IsDeleted(DistanceConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1;}

struct BendingConstraint
{
	ivec4 mEnd; //.xyz = end indices, .w = flags

	float mAlpha;
	float mLambda;
	float mAngle;
	uint mFlags;
};

bool IsDeleted(BendingConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1 || con.mEnd[2]==-1;}

struct AreaConstraint
{
	ivec4 mEnd; //.xyz = end indices, .w = flags

	float mAlpha;
	float mLambda;
	float mArea;
	uint mFlags;
};

bool IsDeleted(AreaConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1 || con.mEnd[2]==-1;}

struct LinearConstraint
{
   ivec4 mEnd;
   
   float mAlpha;
   float mLambda;
   uint mFlags;
   float mPadding0;
};

bool IsDeleted(LinearConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1 || con.mEnd[2]==-1;}
int GetParticle(LinearConstraint con) {return con.mEnd[2];}

struct PlanarConstraint
{
   ivec4 mEnd;
   
   float mAlpha;
   float mLambda;
   uint mFlags;
   float mPadding0;
};

bool IsDeleted(PlanarConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1 || con.mEnd[2]==-1;}
int GetParticle(PlanarConstraint con) {return con.mEnd[2];}

struct StaticPlanarConstraint
{
   ivec4 mEnd; // mEnd[0]: plane index, mEnd[1]: particle index
   
   float mAlpha;
   float mLambda;
   uint mFlags;
   float mPadding0;
};

bool IsDeleted(StaticPlanarConstraint con) {return con.mEnd[0]==-1 || con.mEnd[1]==-1;}
int GetPlane(StaticPlanarConstraint con) {return con.mEnd[0];}
int GetParticle(StaticPlanarConstraint con) {return con.mEnd[1];}

struct RotaryActuator
{
   ivec4 mEnd;

   float mL0;  //TODO remove, there is no built-in distance constraint
   float mAlpha;
   float mOmega;
   uint mFlags;
};

bool IsDeleted(RotaryActuator act)	{return act.mEnd[0]==-1 || act.mEnd[1]==-1;}
ivec2 GetAxis(RotaryActuator act)	{return act.mEnd.xy;}
int GetParticle(RotaryActuator act)	{return act.mEnd[2];}

struct LinearActuator
{
   ivec2 mEnd;
   float mL0;
   float mAlpha;

   float mAngle;
   float mFreq;
   float mAmplitude;
   uint mFlags;
};

bool IsDeleted(LinearActuator act) {return act.mEnd[0]==-1 || act.mEnd[1]==-1;}

struct ConstraintPlane
{
   vec4 mEqn;

   int mGridSquares;
   int mSnapEnabled;
   int mPadding0;
   int mPadding1;
};

struct ShapeMatchingConstraint
{
	mat4 mA;
	mat4 mR;
	vec4 mCen;
	ivec4 mN;
	vec4 mColor;
};

struct ShapeParticle
{
   int mParticleIndex;
   int mShapeConstraintIndex;
   int mPadding0;
   int mPadding1;

   vec4 mRestPos;
};

struct XpbdSimData
{
   vec4 g;
   float dt;
   float c; //damping
   float omega_dist;
   float omega_bend;
   float omega_collision;
};

const int kSimUboBinding = 10;

layout(std140, binding = kSimUboBinding) uniform SimData
{
	vec4 g;
	float dt;
	float c; //damping
	float omega_dist;
	float omega_bend;
	float omega_collision;
} SimUbo;