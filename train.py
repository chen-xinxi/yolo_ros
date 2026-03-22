from ultralytics import YOLO

# 1. 加载模型# 还是加载 n 版本，在这个基础上根据我们的数据进行微调 (Transfer Learning)
model = YOLO('yolo11n.pt') 

# 2. 开始训练# data: 指定刚才写的 yaml 文件# epochs: 训练几轮？# imgsz: 图片大小，一般是 640# device: 有显卡填 0，没显卡填 'cpu'
model.train(data='/home/chen/yolo_use/my_dataset/robocon.yaml', epochs=120, imgsz=640, device=0) 