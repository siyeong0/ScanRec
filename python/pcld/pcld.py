import numpy as np

class Pcld():
    def __init__(self, points: np.ndarray):
        self.points = points
        
    @property
    def xyz(self):
        return self.points[:, 0:3]
    def normal(self):
        return self.points[:, 3:6]
    @property
    def color(self):
        return (self.points[:, 6:9] * 255).astype(np.uint8)
    @property
    def label(self):
        return self.points[:, 9:10]
    
    ##############################################################
    # static methods
    ##############################################################
    def from_rgbdl(rgb, depth, masks, camera_info):
        pass
    
    def from_txt_file(path):
        pass
        