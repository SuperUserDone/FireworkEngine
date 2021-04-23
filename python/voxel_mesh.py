#!/usr/bin/env python3

from pyassimp import *
import numpy as np
from stl import mesh

size = 4

voxel_data = [
    # Layer 1
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 0,
    # Layer 2
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1, 0,
    1, 1, 0, 0,
    # Layer 3
    1, 1, 1, 1,
    1, 1, 1, 0,
    1, 1, 0, 0,
    1, 0, 0, 0,
    # Layer 4:
    1, 1, 1, 0,
    1, 1, 0, 0,
    1, 0, 0, 0,
    0, 0, 0, 0,
]

cube_verts = np.array([
    [0, 0, 0],
    [1, 0, 0],
    [1, 1, 0],
    [0, 1, 0],
    [0, 0, 1],
    [1, 0, 1],
    [1, 1, 1],
    [0, 1, 1],
])

cube_faces = np.array([
    [0, 3, 1],
    [1, 3, 2],
    [0, 4, 7],
    [0, 7, 3],
    [4, 5, 6],
    [4, 6, 7],
    [5, 1, 2],
    [5, 2, 6],
    [2, 3, 6],
    [3, 7, 6],
    [0, 1, 5],
    [0, 5, 4]
])


def build_export(model):
    cube = mesh.Mesh(np.zeros(model["index"].shape[0], dtype=mesh.Mesh.dtype))

    model["index"] = model["index"].reshape(-1, 3)
    model["mesh"] = model["mesh"].reshape(-1, 3)

    for i, f in enumerate(model["index"]):
        for j in range(3):
            cube.vectors[i][j] = model["mesh"][round(f[j])]

    cube.save('cube.stl')


cube_count = 0


def add_cube(i, j, k, model):
    global cube_count
    for l in range(8):
        model["mesh"] = np.append(model["mesh"],
                                  [[cube_verts[l][0] + i, cube_verts[l][1] + j, cube_verts[l][2] + k]])
    for l in range(12):
        model["index"] = np.append(model["index"],
                                   [[cube_faces[l][0] + cube_count*8, cube_faces[l][1] + cube_count*8, cube_faces[l][2] + cube_count*8]])
    cube_count = cube_count + 1


def build_lazy(data, size):
    model = {"mesh": [[]], "index": [[]]}
    for i in range(size):
        for j in range(size):
            for k in range(size):
                if(voxel_data[i+size*(j+k*size)] > 0):
                    add_cube(i, j, k, model)

    return model


def build_greedymesh(data, size):
    pass


model = build_lazy(voxel_data, size)
print(model)

build_export(model)
