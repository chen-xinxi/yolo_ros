from ultralytics import YOLO

# 加载模型并直接运行摄像头检测（一行代码搞定）
YOLO('runs/detect/train/weights/best.pt')(source=0, show=True, conf=0.5)