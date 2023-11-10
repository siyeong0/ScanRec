# Some basic setup:
# Setup detectron2 logger
import detectron2
from detectron2.utils.logger import setup_logger
setup_logger()

# import some common libraries
import numpy as np
import os, json, cv2, random

# import some common detectron2 utilities
from detectron2 import model_zoo
from detectron2.engine import DefaultPredictor
from detectron2.config import get_cfg
from detectron2.utils.visualizer import Visualizer
from detectron2.data import MetadataCatalog, DatasetCatalog

if __name__ == "__main__":
    import torch
    print(torch.cuda.is_available())

    #path = os.path.dirname(__file__) + '/resources/input.png'
    path = "rgb/0000001-000000000000.jpg"
    im = cv2.imread(path)
    if im is not None:
        print('width: {}, height : {}'.format(im.shape[1], im.shape[0]))
    else:
        print('{} not exist.'.format(path))
        exit(1)
        
    cv2.imshow("original image",im)
    cv2.waitKey()

    cfg = get_cfg()
    cfg.merge_from_file(model_zoo.get_config_file("COCO-PanopticSegmentation/panoptic_fpn_R_50_3x.yaml"))
    cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.5  # set threshold for this model
    cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url("COCO-PanopticSegmentation/panoptic_fpn_R_50_3x.yaml")
    cfg.MODEL.DEVICE = "cuda"

    predictor = DefaultPredictor(cfg)
    outputs = predictor(im)

    # look at the outputs. See https://detectron2.readthedocs.io/tutorials/models.html#model-output-format for specification
    #print(outputs["instances"].pred_classes)
    #print(outputs["instances"].pred_boxes)

    classes = outputs["instances"].to("cpu").pred_classes.tolist() if outputs["instances"].to("cpu").has("pred_classes") else None

    # We can use `Visualizer` to draw the predictions on the image.
    v = Visualizer(im[:, :, ::-1], MetadataCatalog.get(cfg.DATASETS.TRAIN[0]), scale=1.2)
    v.metadata.get("thing_classes", None)
    out = v.draw_instance_predictions(outputs["instances"].to("cpu"))
    cv2.imshow("output image",out.get_image()[:, :, ::-1])
    cv2.waitKey()

    cv2.destroyAllWindows()