from pymo.parsers import BVHParser
from tool import load_weight, save_off, save_anim, save_skel

if __name__ == "__main__":
    source = "Demos/Skinning/data/Kaya.fbx"
    bvh_file = "Demos/Skinning/data/Agreeing.bvh"
    weight, label, joints, verts, faces = load_weight(source)

    bvh_parser = BVHParser()
    parsed_data = bvh_parser.parse(bvh_file)

    save_off(verts, faces, "Kaya")
    # save_anim(parsed_data, "Kaya")
    # save_skel()