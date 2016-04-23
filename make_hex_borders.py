# This example assumes we have a mesh object selected

import bpy
import bmesh
import math

# Get the active mesh
me = bpy.context.object.data


# Get a BMesh representation
bm = bmesh.new()   # create an empty BMesh
bm.from_mesh(me)   # fill it in from a Mesh

print('***********************************************')

sin_0 = math.sin(math.radians(0.0))
cos_0 = math.cos(math.radians(0.0))
print(cos_0)
print(sin_0)

sin_60 = math.sin(math.radians(60.0))
cos_60 = math.cos(math.radians(60.0))
print(cos_60)
print(sin_60)

thick = 1.0 - 0.12
thin = 1.0 - 0.06

##############################################################
# Configure these to make the different versions of the quad.
delta = thick
bit_before = False
bit_after = False
##############################################################

bm.verts[0].co.x = cos_0
bm.verts[0].co.y = sin_0
bm.verts[0].co.z = 0

bm.verts[1].co.x = delta * cos_0
bm.verts[1].co.y = delta * sin_0
bm.verts[1].co.z = 0

bm.verts[2].co.x = cos_60
bm.verts[2].co.y = sin_60
bm.verts[2].co.z = 0

bm.verts[3].co.x = delta * cos_60
bm.verts[3].co.y = delta * sin_60
bm.verts[3].co.z = 0

prev_side_dir_x = bm.verts[0].co.x - bm.verts[2].co.x
prev_side_dir_y = bm.verts[0].co.y - bm.verts[2].co.y
length = math.sqrt(prev_side_dir_x * prev_side_dir_x + prev_side_dir_y * prev_side_dir_y)
print('prev_side_dir_x =',prev_side_dir_x)
print('prev_side_dir_y =',prev_side_dir_y)
print(length)
prev_side_dir_x /= length
prev_side_dir_y /= length
print('prev_side_dir_x =',prev_side_dir_x)
print('prev_side_dir_y =',prev_side_dir_y)
next_side_dir_x = -prev_side_dir_x
next_side_dir_y = -prev_side_dir_y

if bit_before:
    bm.verts[1].co.x = bm.verts[0].co.x + prev_side_dir_x * delta
    bm.verts[1].co.y = bm.verts[0].co.y + prev_side_dir_y * delta

if bit_after:
    bm.verts[3].co.x = bm.verts[2].co.x + next_side_dir_x * delta
    bm.verts[3].co.y = bm.verts[2].co.y + next_side_dir_y * delta

# Finish up, write the bmesh back to the mesh
bm.to_mesh(me)
bm.free()  # free and prevent further access
