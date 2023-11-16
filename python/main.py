import numpy as np
import cv2 as cv
import open3d as o3d

#from smot import SMot
from pcd import Pcd

import struct

class Reader:
    def __init__(self, dir, start, end, stride):
        self.dir = dir
        self.idx = start
        self.end = end
        self.stride = stride
        
    def step(self):
        while self.idx < self.end:
            print(self.idx)
            rgb = cv.imread(f"{self.dir}/image/{self.idx}.jpg")
            if rgb is None:
                self.idx += 1
                continue
            rgb = cv.cvtColor(rgb, cv.COLOR_BGR2RGB)
            depth = cv.imread(f"{self.dir}/depth/{self.idx}.png", cv.IMREAD_ANYDEPTH)
            if depth is None:
                self.idx += 1
                continue
            mat_file = open(f"{self.dir}/camera/{self.idx}.bin", "rb")
            cam_intrinsic = np.empty((4,3), dtype=np.float32)
            for i in range(3):
                for j in range(4):
                    cam_intrinsic[j,i] = struct.unpack('<f', mat_file.read(4))[0]
            cam_extrinsic = np.empty((4,4), dtype=np.float32)
            for i in range(4):
                for j in range(4):
                    cam_extrinsic[j,i] = struct.unpack('<f', mat_file.read(4))[0]
            self.idx += self.stride
            
            rgb = cv.resize(rgb, (depth.shape[1], depth.shape[0]))
            cam_intrinsic /= 5
            
            return rgb, depth, cam_intrinsic, cam_extrinsic
        return None
                
if __name__ == "__main__":
    #smot = SMot()

    static_pcd_id = 1023
    pcd_dict = {static_pcd_id: Pcd()}
    reader = Reader("../resources/2023-11-09-17-09-09", 1014, 1700, 1)

    done = False
    while True:
        ret = reader.step()
        if ret is None:
            break
        rgb, depth, cam_intrinsic, cam_extrinsic = ret
        w, h = 256, 144
        cx, cy, fx, fy = cam_intrinsic[0,2], cam_intrinsic[1,2], cam_intrinsic[0,0], cam_intrinsic[1,1]
        Pcd.set_camera_intrinsic(w,h,fx,fy,cx,cy)  
        
        #id_list, class_list, box_list, mask_list = smot.update(rgb)
        static_depth = np.copy(depth)
        #for id, cl, box, mask in zip(id_list, class_list, box_list, mask_list):
        if False:
            id = cl+1
            object_rgb = np.copy(rgb)
            object_depth = np.copy(depth)
            object_depth[np.where(mask == False)] = 0
            static_depth[np.where(mask == True)] = 0
            
            if id in pcd_dict.keys():
                pass
            else:
                pcd_dict[id] = Pcd()
                
            color_table = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,0,255), (0,255,255), (0,0,0)]
            color = color_table[int(id) % len(color_table)]
            curr_pcd = pcd_dict[id].add(object_rgb, object_depth, cam_extrinsic, color)
            
        static_pcd = pcd_dict[static_pcd_id].add(rgb, static_depth, cam_extrinsic)
    pcds = []
    for pcd in pcd_dict.values():
        curr_pcd = pcd.get_o3d_pcd()
        pcds.append(curr_pcd)
        
        if len(curr_pcd.points) < 100:
            continue
        
        # pcd to mesh
        # distances = curr_pcd.compute_nearest_neighbor_distance()
        # avg_dist = np.mean(distances)
        # radius = 3 * avg_dist
        # bpa_mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(curr_pcd,o3d.utility.DoubleVector([radius, radius * 2]))
        
        # bbox = curr_pcd.get_axis_aligned_bounding_box() 
        # mesh = bpa_mesh.crop(bbox)
        
        # o3d.visualization.draw_geometries([curr_pcd])
        # o3d.visualization.draw_geometries([mesh])
    
    print(len(pcds[0].points))
    mesh_frame = o3d.geometry.TriangleMesh.create_coordinate_frame(
    size=0.6, origin=[0, 0, 0])
    pcds.append(mesh_frame)
    o3d.visualization.draw_geometries(pcds)
    o3d.io.write_point_cloud("result", pcds[0], write_ascii=True)
        