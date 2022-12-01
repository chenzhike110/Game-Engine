from utils.tools import load_weight

if __name__ == "__main__":
    source = "data/Kaya.fbx"
    action = "data/Kaya/Agreeing.bvh"
    weight, label, joints, verts, faces = load_weight(source)

    