import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams["savefig.directory"] = "../imgs/voxel"

class PointCloudVoxelGrid():
    def __init__(self, file_path: str, voxel_size: float):
        super(PointCloudVoxelGrid, self).__init__()
        # file_path need to be of .npy file extension
        try:
            self.point_cloud = np.load(file_path)
            print(f"NDIM: {self.point_cloud.ndim}\nSHAPE: {self.point_cloud.shape}")
            self.voxel_size = voxel_size
            print("Point Cloud Loaded Successfully")
        except Exception as e:
            print(f"ERROR\n{e}")

    def point_cloud_to_voxels(self):
        # pc = self.point_cloud.reshape(-1, 3)
        pc = self.point_cloud

        x, y, z = pc[:, 0].ravel(), pc[:, 1].ravel(), pc[:, 2].ravel()
        with open("test.txt", "w") as f:
            f.write(str(x))
        print(f"EXAMPLE\n{pc}")

        self.x_pts, self.y_pts, self.z_pts = x, y, z

        x_min, y_min, z_min = np.min(x), np.min(y), np.min(z)
        x_max, y_max, z_max = np.max(x), np.max(y), np.max(z)

        grid_x = int(np.ceil((x_max-x_min)/self.voxel_size))
        grid_y = int(np.ceil((y_max-y_min)/self.voxel_size))
        grid_z = int(np.ceil((z_max-z_min)/self.voxel_size))

        # initializing voxel grid (seting all to false for now)
        voxels = np.zeros((grid_x, grid_y, grid_z), dtype=bool)

        x_indices = np.floor((x-x_min)/self.voxel_size).astype(np.uint)
        y_indices = np.floor((y-y_min)/self.voxel_size).astype(np.uint)
        z_indices = np.floor((z-z_min)/self.voxel_size).astype(np.uint)

        valid_indices = (x_indices > 0) & (x_indices < grid_x) & \
                        (y_indices > 0) & (y_indices < grid_y) & \
                        (z_indices > 0) & (z_indices < grid_z)
        
        voxels[x_indices[valid_indices], y_indices[valid_indices], z_indices[valid_indices]] = True

        self.voxels: np.ndarray = voxels
        self.min_coords: tuple = (x_min, y_min, z_min)

        # return [voxels, x_min, y_min, z_min]
    
    def visualize_voxels_points(self):
        self.point_cloud_to_voxels()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection="3d")

        x_min, y_min, z_min = self.min_coords
        x, y, z = np.indices(np.array(self.voxels.shape) + 1).astype(float)

        x = x*self.voxel_size + x_min
        y = y*self.voxel_size + y_min
        z = z*self.voxel_size + z_min

        ax.voxels(x, y, z, self.voxels, facecolors="blue", edgecolors="k", alpha=0.3)
        # x_pts, y_pts, z_pts = self.point_cloud.reshape(3, -1)

        pc = self.point_cloud
        # x_pts, y_pts, z_pts = pc[:0].ravel(), pc[:1].ravel(), pc[:2].ravel()
        
        try:
            print(f"PC_SHAPE/DIM: {pc.shape}/{pc.ndim}\n")
            # with open("new_test.txt", "w") as f:
            #     f.write(str(y_pts))
            # print(f"X_PTS_SHAPE/DIM: {x_pts.shape}/{x_pts.ndim}\n")
            # print(f"Y_PTS_SHAPE/DIM: {y_pts.shape}/{y_pts.ndim}\n")
            # print(f"Z_PTS_SHAPE/DIM: {z_pts.shape}/{z_pts.ndim}\n")
            ax.scatter(self.x_pts, self.y_pts, self.z_pts, c="red", marker=".", s=1, label="Original Point Cloud")
        except Exception as e:
            print(f"ERROR\n{e}")

        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        ax.set_title("Voxelization With Point Cloud Overlayed")
        ax.legend()
        plt.savefig("VOXEL_GRID_VISUALIZATION_NEW_TEST_POINT_CLOUD")
        plt.show()

file_path = "./test_point_cloud.npy"
test = PointCloudVoxelGrid(file_path, 0.1)
test.visualize_voxels_points()