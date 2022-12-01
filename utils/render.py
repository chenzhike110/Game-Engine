import math
import pyrender
import trimesh
import numpy as np
from scipy.spatial.transform import Rotation as R

def plotarrow(
        points,
        scene,
        tube_radius=None,
        material=None,
        smooth=True,
    ):

    length = np.linalg.norm(points[1]-points[0], axis=1)
    direction = (points[1]-points[0]) / length.reshape((-1, 1))
    theta = np.arccos(direction[:, 2]).reshape((-1, 1))

    rotateAxis = direction * np.array([-1, 1, 0])

    rotate = R.from_rotvec(rotateAxis* theta) 
    transform = np.zeros((theta.shape[0], 4, 4))
    transform[:, :-1, -1] = points[0]
    transform[:, :-1, :-1] = rotate.as_matrix()
    transform[:, -1, -1] = 1
    for i in range(transform.shape[0]):
        mesh = trimesh.creation.cylinder(tube_radius, 3, transform=transform[i])
        material = pyrender.MetallicRoughnessMaterial(
            metallicFactor=0.0,
            alphaMode='BLEND',
            baseColorFactor=(0.0, 0.0, 1.0, 1.0))
        arrow = pyrender.Mesh.from_trimesh(mesh, smooth=smooth, material=material)
        scene.add(arrow,
                name='grad'+str(i),
                pose=np.eye(4))

    return 
def create_mesh(vertices, faces, face_color=None):
    tri_mesh = trimesh.Trimesh(vertices=vertices, faces=faces, face_colors=face_color)

    return pyrender.Mesh.from_trimesh(
        tri_mesh,
        smooth=False
    )

def create_scene(run_in_thread=True, save_file=None, cam_pose=None, point_size=10):
    cam = pyrender.PerspectiveCamera(yfov=(np.pi / 3.0))
    if cam_pose is None:
        cam_pose = np.array([
            [0.0, 0.0, -1.0, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [1.0, 0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0, 1.0],
        ])@np.array([
            [1.0,  0.0,  0.0,  0.0],
            [0.0,  1.0,  0.0,  100.0],
            [0.0,  0.0,  1.0,  300.0],
            [0.0,  0.0,  0.0,  1.0]
        ])
    scene = pyrender.Scene(bg_color=[0.1, 0.1, 0.1, 0.2],
                            ambient_light=np.array([0.02, 0.02, 0.02, 1.0]))
    scene.add(cam, pose=cam_pose)
    plane = trimesh.creation.box(extents=(300, 0.01, 300))
    material = pyrender.MetallicRoughnessMaterial(baseColorFactor=(255/255, 250/255, 205/255, 1.0), roughnessFactor=0.1)
    plane = pyrender.Mesh.from_trimesh(plane, smooth=True, material=material)
    scene.add(plane)
    viewer = pyrender.Viewer(scene, use_raymond_lighting=True,
                                viewport_size=(1200, 800),
                                cull_faces=False,
                                run_in_thread=run_in_thread,
                                point_size=point_size
                            )
    if save_file is not None:
        viewer.viewer_flags['record']=True
    return viewer, scene

def plot_grad(viewer, scene, vertices, grad, thre=1e-4):
    viewer.render_lock.acquire()
    grad = grad.clone().detach().cpu().numpy().reshape((-1, 3))
    start = vertices.clone().detach().cpu().numpy().reshape((-1, 3))[np.linalg.norm(grad,axis=1) > thre]
    grad = grad[np.linalg.norm(grad,axis=1) > thre]
    end = start + grad
    plotarrow([start, end], scene, tube_radius=0.1)
    viewer.render_lock.release()

def visualize(viewer, scene, np_verts, faces, intr_faces=[], recv_faces=[], alpha=1.0):
    viewer.render_lock.acquire()

    # clean mesh
    for node in scene.get_nodes():
        if node.name is None:
            continue
        if any([query in node.name for query in ['recv_mesh', 'intr_mesh', 'body_mesh', 'grad']]):
            scene.remove_node(node)

    curr_verts = np_verts.copy()
    face_colors = np.zeros((len(faces), 4))
    face_colors[:, :-1] = 0.3
    face_colors[:, -1] = alpha
    face_colors[intr_faces] = [0.9, 0.0, 0.0, alpha]
    face_colors[recv_faces] = [0.0, 0.9, 0.0, alpha]
    body_mesh = create_mesh(curr_verts, faces,
                            face_color=face_colors)

    pose = np.eye(4)
    scene.add(body_mesh,
                name='body_mesh',
                pose=pose)

    # if len(intr_faces) > 0:
    #     intr_mesh = create_mesh(curr_verts, intr_faces,
    #                             color=(0.9, 0.0, 0.0, alpha))
    #     scene.add(intr_mesh,
    #                 name='intr_mesh',
    #                 pose=pose)

    # if len(recv_faces) > 0:
    #     recv_mesh = create_mesh(curr_verts, recv_faces,
    #                             color=(0.0, 0.9, 0.0, alpha))
    #     scene.add(recv_mesh, name='recv_mesh',
    #                 pose=pose)

    viewer.central_node = body_mesh
    viewer.render_lock.release()