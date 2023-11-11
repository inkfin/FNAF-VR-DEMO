layout(std140, binding = kGridUboBinding) uniform UniformGridInfo3D
{
	aabb3D mExtents;
	ivec4 mNumCells;
	vec4 mCellSize;
	int mMaxCellsPerElement;
};

ivec3 CellCoord(vec3 p)
{
   p = p-mExtents.mMin.xyz;
   ivec3 cell = ivec3(floor(p/mCellSize.xyz));
   cell = clamp(cell, ivec3(0), mNumCells.xyz-ivec3(1));
   return cell;
}

int Index(ivec3 coord)
{
	return coord.x + mNumCells.x*(coord.y + mNumCells.y * coord.z);
}

ivec2 ContentRange(ivec3 cell)
{
	int cell1 = Index(cell);
	int start = mStart[cell1];
	int count = mCount[cell1];
	return ivec2(start, start+count-1);
}