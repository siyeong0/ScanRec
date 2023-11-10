import os
import numpy as np
import cv2 as cv
import open3d as o3d
import struct
import sys
sys.path.append(os.path.abspath("python"))
from smot import SMot

if __name__ == "__main__":
    cam_intrinsic = o3d.camera.PinholeCameraIntrinsic(o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault)
    
    rgbs = os.listdir("rgb")
    rgbs.sort()
    depths = os.listdir("depth")
    depths.sort()
    
    smot = SMot()
    
    pcds = []
    for i in range(1):
        rgb_path = "rgb/"+rgbs[i]
        depth_path = "depth/"+depths[i]
        
        dd = cv.imread(depth_path, 2)
        color_raw = o3d.io.read_image(rgb_path)
        depth_raw = o3d.io.read_image(depth_path)
        rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
            color_raw, depth_raw, convert_rgb_to_intensity=False)

        pcd = o3d.geometry.PointCloud.create_from_rgbd_image(rgbd_image, cam_intrinsic)
        pcds.append(pcd)
        o3d.visualization.draw_geometries(pcds)
    o3d.visualization.draw_geometries(pcds)
        
        
        
        
        
    # mat_file = open(mat_path, "rb")
    # cam_mat = np.empty((4,3), dtype=np.float32)
    # for i in range(3):
    #     for j in range(4):
    #         cam_mat[j,i] = struct.unpack('f', mat_file.read(4))[0]
    # view_mat = np.empty((4,4), dtype=np.float32)
    # for i in range(4):
    #     for j in range(4):
    #         view_mat[j,i] = struct.unpack('f', mat_file.read(4))[0]