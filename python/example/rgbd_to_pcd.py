import numpy as np
import open3d as o3d

if __name__ == "__main__":
    color_raw = o3d.io.read_image("redwood/rgb/0000001-000000000000.jpg")
    depth_raw = o3d.io.read_image("redwood/depth/0000001-000000000000.png")
    rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
        color_raw, depth_raw)

    pcd = o3d.geometry.PointCloud.create_from_rgbd_image(
        rgbd_image,
        o3d.camera.PinholeCameraIntrinsic(
            o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault))
    # Flip it, otherwise the pointcloud will be upside down
    pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])

    #pcd.colors = o3d.utility.Vector3dVector(np.array(pcd.normals) * 255)
    o3d.visualization.draw_geometries([pcd])
    
    # pcd.estimate_normals()
    # pcd.colors = o3d.utility.Vector3dVector(np.array(pcd.normals) * 255)
    # o3d.visualization.draw_geometries([pcd])
    