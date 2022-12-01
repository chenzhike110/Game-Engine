import os
import numpy as np
import pandas as pd
import bpy
import bmesh
import torch
import trimesh
from mesh_to_sdf import get_surface_point_cloud
from pymo.parsers import BVHParser
from pymo.writers import BVHWriter

from scipy.spatial.transform import Rotation

def node_parent(parsed_data, joints_name, joints_index):
    parent = [joints_index[parsed_data.skeleton[joint]['parent']] if parsed_data.skeleton[joint]['parent'] in joints_name # root if no parent in joints name
              else -1 for joint in joints_name]
    return torch.LongTensor(parent)

def node_offset(parsed_data, joints_name):
    offset = torch.stack([torch.Tensor(parsed_data.skeleton[joint]['offsets']) for joint in joints_name], dim=0)
    return offset

def get_topology_from_bvh(parsed_data):
    joints_name = []
    for joint in parsed_data.skeleton:
        if "_Nub" not in joint:
            joints_name.append(joint)
    return joints_name

def joint_angles(parsed_data, t, joints_name, joints_index, num_nodes):
    # collect joint angles
	rotation_t = parsed_data.values.iloc[t, :]
	joint_X = {joint: None for joint in joints_name}
	joint_Y = {joint: None for joint in joints_name}
	joint_Z = {joint: None for joint in joints_name}
	for name, rot in rotation_t.items():
		joint, rotation = name.rsplit('_', 1)
		if joint not in joints_name:
			continue
		if rotation == 'Xrotation':
			joint_X[joint] = rot*np.pi/180.0
		elif rotation == 'Yrotation':
			joint_Y[joint] = rot*np.pi/180.0
		else:
			joint_Z[joint] = rot*np.pi/180.0
	x = [None for _ in range(num_nodes)]
	for joint in joints_name:
		euler_angles = torch.tensor([joint_X[joint], joint_Y[joint], joint_Z[joint]])
		x[joints_index[joint]] = euler_angles
	x = torch.stack(x, dim=0)
	return x

def load_angle(sourse):
	bvh_parser = BVHParser()
	parsed_data = bvh_parser.parse(sourse)
	joints_all = get_topology_from_bvh(parsed_data)
	joints_index = {name: i for i, name in enumerate(joints_all)}
	parent_all = node_parent(parsed_data, joints_all, joints_index)
	offset_all = node_offset(parsed_data, joints_all)
	total_frames = parsed_data.values.shape[0]
	data_list = []
	for t in range(0, total_frames):
		ang = joint_angles(parsed_data, t, joints_all, joints_index, len(joints_all))
		data_list.append(ang)
	return data_list, parent_all, offset_all, joints_all

def create_folder(folder):
	if not os.path.exists(folder):
		os.makedirs(folder)

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
	
def linear_blend_skinning(weights, joint_origin, joint_new, verticles_origin):
	"""
    Linear blend skinning
    verticle = \Sigma weight * skeletonM * verticle_origin
    
    Parameters:
    	weights: skinning weights V x J
    	joint_origin: origin transform matrix of skeleton joint J x 4 x 4
		joint_new: new transform matrix of skeleton joint J x 4 x 4
    	verticle_origin: origin transform of verticle V x 4 x 1
	Return:
		vertices_new: new transform of verticle V x 4 x 1
	"""
	skeletonM = torch.tensor(joint_new @ np.linalg.inv(joint_origin), dtype=torch.float)
	transform = torch.einsum('ij,jkl->ikl', [weights, skeletonM])
	vertice_new = torch.bmm(transform, verticles_origin)
	return vertice_new

def write_bvh(src_file, trg_file, out_file, root_pos, ang, joints_index):
	bvh_parser = BVHParser()
	source_data = bvh_parser.parse(src_file)
	target_data = bvh_parser.parse(trg_file)
	index = source_data.values.index
	columns = target_data.values.columns
	# print(index, columns)
	new_values = pd.DataFrame(data=np.zeros((len(index), len(columns))), columns=columns, index=index)
	new_values['Hips_Xposition'] = root_pos[:, 0]
	new_values['Hips_Yposition'] = root_pos[:, 1]
	new_values['Hips_Zposition'] = root_pos[:, 2]
	for joint, index in joints_index.items():
		# print(new_values[joint+'_Xrotation'])
		new_values[joint+'_Xrotation'] = np.degrees(ang[:, index, 0])
		new_values[joint+'_Yrotation'] = np.degrees(ang[:, index, 1])
		new_values[joint+'_Zrotation'] = np.degrees(ang[:, index, 2])
	target_data.values = new_values
	bvh_writer = BVHWriter()
	with open(out_file, 'w') as ofile:
		bvh_writer.write(target_data, ofile)

def generate_watertight_mask(vertices, faces):
	origin_mesh = trimesh.Trimesh(vertices=vertices, faces=faces)
	subpart = origin_mesh.split(only_watertight=False)
	MeshMask = np.zeros(vertices.shape[0])
	for index, verts in enumerate(vertices):
		for meshIndex, subMesh in enumerate(subpart):
			if verts in subMesh.vertices:
				MeshMask[index] = meshIndex+1
				break
	return MeshMask

def remove_collision_withMask(collision_idxs, faces, MeshMask):
	coll_idxs = collision_idxs[:, :, 0].ge(0).nonzero().detach().cpu().numpy()
	for i in range(len(coll_idxs)):
		receiver = collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 0].cpu().numpy().item()
		intruder = collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 1].cpu().numpy().item()
		receiver_point = faces[receiver]
		intruder_point = faces[intruder]
		MeshSet = set(list(MeshMask[receiver_point])+list(MeshMask[intruder_point]))
		if -1 in MeshSet and 0 not in MeshSet:
			pass
		# elif len(MeshSet) > 1 or 0 in MeshSet:
		else:
			collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 0] = -1
			collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 1] = -1
		
	return collision_idxs

def generate_surface_mask(vertices, faces, scan_count=200, scan_resolution=600, distance_threshold=-0.1):
	mesh = trimesh.Trimesh(vertices=vertices, faces=faces)
	sdf = get_surface_point_cloud(mesh, scan_count=scan_count, scan_resolution=scan_resolution)
	np_verts = np.array(mesh.vertices)
	distance = sdf.get_sdf(np_verts)
	MeshMask = np.zeros(np_verts.shape[0])
	MeshMask[distance >= distance_threshold] = 1
	return MeshMask

def update_ee_mask(target, mask):
	weights = target.skinning_weights
	ee_mask = ["Arm" in label or "Leg" in label or "Hand" in label or "Foot" in label for label in target.skinning_label]
	ee_weight = np.sum(weights[:, ee_mask], axis=-1)
	mask[ee_weight > 0] = -1
	return mask
		
def remove_collision_withweight(collision_idxs, faces, weights):
	coll_idxs = collision_idxs[:, :, 0].ge(0).nonzero().detach().cpu().numpy()
	for i in range(len(coll_idxs)):
		receiver = collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 0].cpu().numpy().item()
		intruder = collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 1].cpu().numpy().item()
		receiver_point = faces[receiver]
		intruder_point = faces[intruder]
		receiver_weight = np.sum(weights[receiver_point], axis=0)
		intruder_weight = np.sum(weights[intruder_point], axis=0)
		if receiver_weight @ intruder_weight.T > 0.25:
			collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 0] = -1
			collision_idxs[coll_idxs[i, 0], coll_idxs[i, 1], 1] = -1
	return collision_idxs