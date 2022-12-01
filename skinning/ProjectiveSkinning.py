import trimesh
import numpy as np
from collections import defaultdict
from .kinematics import ForwardKinematics
from utils.render import create_scene, visualize

class ProjectiveSkinning:
    """
    Projective skinning
    """
    def __init__(self, skinning_joints, joints_parents, vertices_Tpose, skinning_weights, faces) -> None:
        self.fk = ForwardKinematics()
        
        

class ConvexCharcter(object):
    """
    Convex Model built from mesh
    """
    def __init__(self, weight, label, joints, verts, faces) -> None:
        
        self.origin_mesh = trimesh.Trimesh(vertices=verts, faces=faces)
        # get vertice joint label
        self.label = [i.split(':')[-1] for i in label]
        self.verg_unique = defaultdict(list)
        self.verg_joint = defaultdict(list)
        self.joint_radius = defaultdict(int)
        self.joint_pos = joints[:, :-1, -1]
        self.meshes = []
        for i in range(weight.shape[0]):
            group = ""
            for j in range(weight.shape[1]):
                if weight[i, j]>0:
                    group += str(j) + "_"
                    self.verg_joint[j].append(verts[i])
            self.verg_unique[group].append(verts[i])
        # print(self.verg.keys())
        for key, value in self.verg_joint.items():
            distance = np.array([np.linalg.norm(i-joints[key, :-1, -1]) for i in value])
            self.joint_radius[key] = np.min(distance)
        # print(self.joint_radius)
        for key, value in self.joint_radius.items():
            sphere = trimesh.primitives.Sphere(radius=value*0.75, center=self.joint_pos[key])
            self.meshes.append(sphere)
        self.combined = trimesh.util.concatenate(self.meshes)
    
    def show_weights(self):
        viewer, scene = create_scene()
        visualize(viewer, scene, np.array(self.origin_mesh.vertices), np.array(self.origin_mesh.faces), alpha=0.4)
        import pyrender
        viewer.render_lock.acquire()
        lines = []
        for key, value in self.verg_joint.items():
            for point in value:
                line = pyrender.Primitive([point, self.joint_pos[key]], mode=1)
                lines.append(line)
        scene.add(pyrender.Mesh(lines))
        viewer.render_lock.release()