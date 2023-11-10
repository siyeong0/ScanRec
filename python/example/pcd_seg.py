from detectron2.utils.logger import setup_logger
setup_logger()

import os
import numpy as np
import cv2
import sys

from detectron2 import model_zoo
from detectron2.engine import DefaultPredictor
from detectron2.config import get_cfg

sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from sort_tracking import Sort

import open3d as o3d

if __name__ == "__main__":
    rgb = cv2.imread("rgb/0000001-000000000000.jpg")
    depth = cv2.imread("depth/0000001-000000000000.png", cv2.IMREAD_ANYDEPTH)
    
    cfg = get_cfg()
    cfg.merge_from_file(model_zoo.get_config_file("COCO-PanopticSegmentation/panoptic_fpn_R_50_3x.yaml"))
    cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5  # set threshold for this model
    cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url("COCO-PanopticSegmentation/panoptic_fpn_R_50_3x.yaml")
    cfg.MODEL.DEVICE = "cuda"
    predictor = DefaultPredictor(cfg)
    tracker = Sort()
    
    while True:
        outputs = predictor(rgb)
        out_data = outputs["instances"].to("cpu").get_fields()
        boxes = out_data['pred_boxes'].tensor.numpy()
        scores = out_data['scores'].numpy()
        classes = out_data['pred_classes'].numpy()
        masks = out_data['pred_masks'].numpy()
           
        dets = np.empty((len(scores), 5))
        dets[:, 0:4] = boxes
        dets[:, 4] = scores
        track_info = tracker.update(dets)
        
        color_table = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,0,255), (0,255,255), (0,0,0)]
        pcds = []
        static_depth = np.copy(depth)
        for i in range(track_info.shape[0]):
            tr_box = track_info[i][0:4]
            tr_id = track_info[i][4]
            color = color_table[int(tr_id) % len(color_table)]
            
            idx = np.where(np.array([np.allclose(tr_box, box, atol=30) for box in boxes]))
            if len(idx[0]) == 0:
                continue
            idx = idx[0][0]
            
            # create pcd
            box = boxes[idx].astype(int)
            mask = masks[idx]
            object_rgb = np.copy(rgb)
            object_depth = np.copy(depth)
            object_depth[np.where(mask == False)] = 0
            static_depth[np.where(mask == True)] = 0

            color_raw = o3d.geometry.Image(object_rgb)
            depth_raw = o3d.geometry.Image(object_depth)
            rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
                color_raw, depth_raw, depth_scale=1.0, depth_trunc = 10000.0, convert_rgb_to_intensity=False)

            pcd = o3d.geometry.PointCloud.create_from_rgbd_image(
                rgbd_image,
                o3d.camera.PinholeCameraIntrinsic(
                    o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault))
            # Flip it, otherwise the pointcloud will be upside down
            pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])

            pcd.colors = o3d.utility.Vector3dVector(np.array(pcd.colors) * 0.75 + np.array(color) / 255 * 0.25)
            
            pcd = pcd.remove_duplicated_points()
            pcd.estimate_normals()
            #pcd = pcd.remove_radius_outlier(4, 0.000001)[0]
            pcds.append(pcd)
            
            if not pcd.has_points():
                continue
            
            #poisson_mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_poisson(pcd, depth=8, width=0, scale=1.1, linear_fit=False)[0]
            distances = pcd.compute_nearest_neighbor_distance()
            avg_dist = np.mean(distances)
            radius = 3 * avg_dist
            bpa_mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(pcd,o3d.utility.DoubleVector([radius, radius * 2]))
            
            bbox = pcd.get_axis_aligned_bounding_box() 
            p_mesh_crop = bpa_mesh.crop(bbox)

            #dec_mesh = p_mesh_crop.simplify_quadric_decimation(100000)
            #dec_mesh.remove_degenerate_triangles()
            #dec_mesh.remove_duplicated_triangles()
            #dec_mesh.remove_duplicated_vertices()
            #dec_mesh.remove_non_manifold_edges()

            #o3d.visualization.draw_geometries([pcd], point_show_normal=True)
            #o3d.visualization.draw_geometries([p_mesh_crop])
            
        # static objects
        color_raw = o3d.geometry.Image(rgb)
        depth_raw = o3d.geometry.Image(static_depth)
        rgbd_image = o3d.geometry.RGBDImage.create_from_color_and_depth(
            color_raw, depth_raw, depth_scale=1.0, depth_trunc = 10000.0, convert_rgb_to_intensity=False)

        pcd = o3d.geometry.PointCloud.create_from_rgbd_image(
            rgbd_image,
            o3d.camera.PinholeCameraIntrinsic(
                o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault))
        # Flip it, otherwise the pointcloud will be upside down
        pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])

        for p in pcds:
            if pcd.has_points():
                pcd.points.extend(p.points)
                pcd.normals.extend(p.normals)
                pcd.colors.extend(p.colors)

        pcd.estimate_normals()
        #o3d.visualization.draw_geometries([pcd], point_show_normal=True)
        o3d.visualization.draw_geometries([pcd])
        #pcds.append(pcd)
        #o3d.visualization.draw_geometries(pcds)