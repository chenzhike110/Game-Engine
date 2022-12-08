import bpy
import bmesh
import numpy as np
from scipy.spatial.transform import Rotation

def clean_scene():
	"""
	Clean blender scene
	"""
	bpy.ops.object.select_all(action='SELECT')
	bpy.ops.object.delete()

def extract_weight(meshes):
	"""
    Extract skinning information from a given mesh

	Parameters:
		mesh: Mesh to be extracted
	Return:
		weight: VxJ weight matrix
		vgrap_label: joint labels for every row of weight matrix
		coords: Vx3 vertices position
		faces: Fx3 vertices index of faces in mesh
	"""
	vgrp_labels = []
	num_vertices = 0
	for me in meshes:
		vgrps = me.vertex_groups
		verts = me.data.vertices
		vgrp_label = vgrps.keys()
		vgrp_labels = list(set(vgrp_labels+vgrp_label))
		num_vertices += len(verts)

	coords = []
	faces = []
	weight = np.zeros((num_vertices, len(vgrp_labels)))

	for me in meshes:
		verts = me.data.vertices
		bm = bmesh.new()
		bm.from_mesh(me.data)
		vgrps = me.vertex_groups
		
		vgrp_label = vgrps.keys()
		face = []

		for i, vert in enumerate(verts):
			for g in vert.groups:
				j = vgrp_labels.index(vgrps.keys()[g.group])
				weight[i+len(coords), j] = g.weight
		
		for f in bm.faces:
			Quadrilateral = [v.index for v in f.verts]
			if len(Quadrilateral) == 4:
				face.append(Quadrilateral[:-1])
				tmp = Quadrilateral[2:]
				tmp.append(Quadrilateral[0])
				face.append(tmp)
			elif len(Quadrilateral) == 3:
				face.append(Quadrilateral)
				# raise ValueError("Fuck Mesh")
			else:
				raise ValueError("Mesh Invalid")
		
		face = list(np.array(face) + len(coords))
		if len(faces) == 0:
			faces = face
		else:
			faces = list(np.append(faces, face, axis=0))
		if len(coords) == 0:
			coords = [v.co for v in me.data.vertices]
		else:
			coords = list(np.append(coords, [v.co for v in me.data.vertices], axis=0))
		
	return weight, vgrp_labels, np.array(coords), np.array(faces)

def extract_joint_transform_fbx(frame, label):
	"""
	Extract joint transform matrix from fbx
	
	Parameters:
		frame: the frame in charameter animation
		label: list of joints' names
	Return:
		joints: Jx4x4 Transformation matrix
	"""
	# set frame
	bpy.context.scene.frame_set(frame)
	
	joints = np.zeros((len(label), 4, 4))
	
	# get bone struct
	bone_struct = bpy.data.objects['Armature'].pose.bones
	for name in label:
		matrix = bone_struct[name].matrix
		joints[label.index(name)] = matrix
	return joints

def load_weight(source):
	"""
	Load Mesh weights, vertices and faces from fbx file

	Parameters:
		source: the path of the fbx file
	Return:
		weight: VxJ weight matrix
		label: Jx1 joint labels for every row of weight matrix
		joints: Jx4x4 Transformation matrix
		verts: Vx3 vertices position
		faces: Fx3 vertices index of faces in mesh
	"""
	clean_scene()
	bpy.ops.import_scene.fbx(filepath=source, use_anim=False)

	me = []
	
	# find mesh
	bpy.ops.object.join()

	for obj in bpy.data.objects:
		if obj.type == 'MESH':
			me.append(obj)
	
	weight, label, verts, faces = extract_weight(me)
	joints = extract_joint_transform_fbx(0, label)
	clean_scene()

	return weight, label, joints, verts, faces

def save_off(vertices, faces, filename):
	output = open(filename+'.off', "w")
	output.write('OFF\n')
	output.write(' '.join([str(vertices.shape[0]), str(faces.shape[0]), '0'])+'\n')
	output.close()
	# 
	faces2 = np.zeros((faces.shape[0], faces.shape[1]+1))
	faces2[:,1:] = faces
	faces2[:, 0] = 3
	with open(filename+'.off', "ab") as f:
		np.savetxt(f, vertices, fmt='%1.10f')
		np.savetxt(f, faces2, fmt='%d')

def save_skel(target, pos, filename):
	pos = list(pos.cpu().numpy())
	output = open(filename+'.skel', "w")
	joints_name = target.joints_all
	parent_all = target.parent_all.numpy()
	dumps = []
	for index, i in enumerate(joints_name):
		if parent_all[index] >= 0:
			parent = target.joints_all[parent_all[index]]
		else:
			parent = 'root'
		dumps.append([str(x) for x in pos[index]]+[i, parent])
	output.write(str(len(dumps))+'\n')
	for line in dumps:
		output.write(' '.join([str(x) for x in line])+'\n')
	output.close()

def save_anim(trans0, trans, filename):
	output = open(filename+'.anim', "w")
	# joints_name = parsed_data.skeleton.keys()
	assert trans0.shape[1] == trans.shape[1]
	output.write(str(trans.shape[0])+' '+str(trans.shape[1])+'\n')
	for i in range(trans.shape[1]):
		output.write("1 0 0\n")
	output.write(' '.join([str(0) for i in range(trans.shape[1])])+"\n")
	for i in range(trans.shape[0]):
		axises = []
		angles = []
		for j in range(trans.shape[1]):
			rotation_matrix = trans[i, j, :, :]
			r = Rotation.from_matrix(rotation_matrix)
			rotvec = r.as_rotvec()
			if np.linalg.norm(rotvec) <= 1e-9:
				angles.append(0)
				axises.append(np.array([1,0,0]))
			else:
				angles.append(np.linalg.norm(rotvec))
				axises.append(rotvec / np.linalg.norm(rotvec))
		output.write(str(i+1)+"\n")
		axises = np.array(axises)
		np.savetxt(output, axises, fmt='%1.10f')
		output.write(' '.join([str(i) for i in angles])+'\n')
	output.close()