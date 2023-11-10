import numpy as np
import open3d as o3d
from scipy.spatial.transform import Rotation as R

color_table = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,0,255), (0,255,255), (0,0,0)]
class Pcd:
    pinhole_intrinsic = o3d.camera.PinholeCameraIntrinsic(o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault)
    def __init__(self, 
                 depth_scale = (2**16 - 1)/8):
        self.pcd = o3d.geometry.PointCloud()
        self.depth_scale = depth_scale
        
    def add(self, rgb: np.ndarray, depth: np.ndarray, camera_transform: np.ndarray, color = None):
        depth[np.where(depth < 16)] = 0
        color_raw = o3d.geometry.Image(rgb)
        depth_raw = o3d.geometry.Image(depth)
        rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
            color_raw, 
            depth_raw, 
            depth_scale = self.depth_scale, 
            depth_trunc = 8.0, 
            convert_rgb_to_intensity=False)

        pcd = o3d.geometry.PointCloud.create_from_rgbd_image(
            rgbd_image,
            self.pinhole_intrinsic)
        
        points = np.asarray(pcd.points)
        points = points * np.array([1.0,-1.0,-1.0])
        buf = np.empty((points.shape[0], 4))
        buf[:, 0:3] = points
        buf[:,3] = 1.0
        points = (buf @ camera_transform.T)[:, 0:3]
        
        pcd.points = o3d.utility.Vector3dVector(points)
        
        pcd.estimate_normals()

        if color != None:
            pcd.colors = o3d.utility.Vector3dVector(np.array(pcd.colors) * 0.75 + np.array(color) / 255 * 0.25)
        # pcd = pcd.remove_duplicated_points()
        
        pcd.remove_non_finite_points()
        pcd.remove_duplicated_points()
        
        self.pcd.points.extend(pcd.points)
        self.pcd.normals.extend(pcd.normals)
        self.pcd.colors.extend(pcd.colors)
        #self.pcd.remove_duplicated_points()
        return pcd

    def get_o3d_pcd(self):
        return self.pcd
    
    def set_camera_intrinsic(w, h, fx, fy, cx, cy):
        Pcd.pinhole_intrinsic = o3d.camera.PinholeCameraIntrinsic(w, h, fx, fy, cx, cy)
        