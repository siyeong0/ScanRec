import os
import numpy as np
import open3d as o3d

if __name__ == "__main__":
    path = os.path.dirname(__file__) + '/resources/sample_w_normals.xyz'
    point_cloud= np.loadtxt(path,skiprows=1)

    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(point_cloud[:,:3])
    pcd.colors = o3d.utility.Vector3dVector(point_cloud[:,3:6]/255)
    pcd.normals = o3d.utility.Vector3dVector(point_cloud[:,6:9])
    o3d.visualization.draw_geometries([pcd])

    # distances = pcd.compute_nearest_neighbor_distance()
    # avg_dist = np.mean(distances)
    # radius = 3 * avg_dist
    # bpa_mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(pcd,o3d.utility.DoubleVector([radius, radius * 2]))
    # o3d.visualization.draw_geometries([bpa_mesh])

    poisson_mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_poisson(pcd, depth=8, width=0, scale=1.1, linear_fit=False)[0]
    bbox = pcd.get_axis_aligned_bounding_box() 
    p_mesh_crop = poisson_mesh.crop(bbox)

    dec_mesh = p_mesh_crop.simplify_quadric_decimation(100000)
    dec_mesh.remove_degenerate_triangles()
    dec_mesh.remove_duplicated_triangles()
    dec_mesh.remove_duplicated_vertices()
    dec_mesh.remove_non_manifold_edges()

    o3d.visualization.draw_geometries([dec_mesh])