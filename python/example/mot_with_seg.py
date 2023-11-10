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

if __name__ == "__main__":
    path = os.path.dirname(__file__) + '/resources/input.mp4'
    cap = cv2.VideoCapture(path)
    if cap.isOpened():
        print('width: {}, height : {}'.format(cap.get(3), cap.get(4)))
    else:
        print('{} not exist.'.format(path))
        exit(1)
        
    cfg = get_cfg()
    cfg.merge_from_file(model_zoo.get_config_file("COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml"))
    cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5  # set threshold for this model
    cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url("COCO-InstanceSegmentation/mask_rcnn_R_50_FPN_3x.yaml")
    cfg.MODEL.DEVICE = "cuda"
    predictor = DefaultPredictor(cfg)
    tracker = Sort()
    
    #while(cap.isOpened()):
    for i in range(0, 320):
        ret, im = cap.read()
        im = cv2.resize(im, (int(cap.get(3)*0.2), int(cap.get(4)*0.2)))
        
        dir = "2023-11-08-12-02-30"
        im = cv2.imread(f"{dir}/image/{i}.jpg")
        if im is None:
            continue
        
        outputs = predictor(im)
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
        
        for i in range(track_info.shape[0]):
            tr_box = track_info[i][0:4]
            tr_id = track_info[i][4]
            color = color_table[int(tr_id) % len(color_table)]
            
            idx = np.where(np.array([np.allclose(tr_box, box, atol=30) for box in boxes]))
            if len(idx[0]) == 0:
                continue
            idx = idx[0][0]
            
            # draw bounding box
            box = boxes[idx]
            im = cv2.rectangle(im, box[0:2].astype(int), box[2:4].astype(int), color)
            # draw sementic segment
            mask = masks[idx]
            tar = im[np.where(mask==True)].astype(np.float32)
            tar *= 0.5
            tar += np.array(color) * 0.5
            im[np.where(mask==True)] = tar.astype(np.uint8)

        
        cv2.imshow("output video",im)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cap.release()
    cv2.destroyAllWindows()