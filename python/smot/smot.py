from detectron2.utils.logger import setup_logger
setup_logger()

import os
import numpy as np
import cv2
import sys

from detectron2 import model_zoo
from detectron2.engine import DefaultPredictor
from detectron2.config import get_cfg

from sort_tracking import Sort

DEFAULT_CFG_FILE = "COCO-PanopticSegmentation/panoptic_fpn_R_50_3x.yaml"
class SMot:
    def __init__(self, cfg_file=DEFAULT_CFG_FILE):
        cfg = get_cfg()
        cfg.merge_from_file(model_zoo.get_config_file(cfg_file))
        cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5  # set threshold for this model
        cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url(cfg_file)
        cfg.MODEL.DEVICE = "cuda"
        self.cfg = cfg
        self.reset()
    
    def reset(self):
        self.predictor = DefaultPredictor(self.cfg)
        self.tracker = Sort()
        
    def update(self, frame):
        id_list = []
        class_list = []
        box_list = []
        mask_list = []
        
        outputs = self.predictor(frame)
        out_data = outputs["instances"].to("cpu").get_fields()
        boxes = out_data['pred_boxes'].tensor.numpy()
        scores = out_data['scores'].numpy()
        classes = out_data['pred_classes'].numpy()
        masks = out_data['pred_masks'].numpy()
           
        dets = np.empty((len(scores), 5))
        dets[:, 0:4] = boxes
        dets[:, 4] = scores
        track_info = self.tracker.update(dets)
        
        for i in range(track_info.shape[0]):
            tr_box = track_info[i][0:4]
            tr_id = track_info[i][4]
            
            idx = np.where(np.array([np.allclose(tr_box, box, atol=30) for box in boxes]))
            if len(idx[0]) == 0:
                continue
            idx = idx[0][0]
            
            id_list.append(tr_id)
            class_list.append(classes[idx])
            box_list.append(boxes[idx])
            mask_list.append(masks[idx])
            
        return (id_list, class_list, box_list, mask_list)
    
    ##############################################################
    # static methods
    ##############################################################
    def visualize(im, id_list, class_list, box_list, mask_list):
        rim = np.array(im, copy=True)
        color_table = [(255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,0,255), (0,255,255), (0,0,0)]
        for id, claas, box, mask in zip(id_list, class_list, box_list, mask_list):
            color = color_table[int(id) % len(color_table)]
            # draw bounding box
            rim = cv2.rectangle(rim, box[0:2].astype(int), box[2:4].astype(int), color)
            # draw sementic segment
            tar = rim[np.where(mask==True)].astype(np.float32)
            tar *= 0.5
            tar += np.array(color) * 0.5
            rim[np.where(mask==True)] = tar.astype(np.uint8)
            
        return rim