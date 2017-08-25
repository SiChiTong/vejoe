# colorTracker - By: Administrator - 周二 八月 15 2017
# Blob Detection Example
#
# This example shows off how to use the find_blobs function to find color
# blobs in the image. This example in particular looks for dark green objects.

import sensor, image, time

# For color tracking to work really well you should ideally be in a very, very,
# very, controlled enviroment where the lighting is constant...
green_threshold   = (   0,   80,  -70,   -10,   -0,   30)
#设置绿色的阈值，括号里面的数值分别是L A B 的最大值和最小值（minL, maxL, minA,
# maxA, minB, maxB），LAB的值在图像左侧三个坐标图中选取。如果是灰度图，则只需
#设置（min, max）两个数字即可。

# You may need to tweak the above settings for tracking green things...
# Select an area in the Framebuffer to copy the color settings.

#转弯
turnVelocity=0;
turnDeltaLast=0;
turnDeltaSum=0;
KP_TURN=5;
KI_TURN=1;
KD_TURN=2;
#直行
moveVelocity=0;
moveDeltaLast=0;
moveDeltaSum=0;
KP_MOVE=5;
KI_MOVE=1;
KD_MOVE=2;
#采样时间间隔
DELTA_TIME_CHECK=2;
#图像整体宽度
IMAGE_WIDTH = 640;
#标准距离是图像尺寸
IMAGE_AREA_SIZE_DEFAULT=100;
#有效像素面积
AREA_PIXEL_MIN=500;
#图像被认为无有效目标前，需要无效目标持续帧数
NONE_TARGET_COUNT_MIN = 5;
#图像中无有效颜色持续的图像帧数
noneTargetLastCount=0;
#实际追踪的目标索引
targetIdx=-1;


sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # use RGB565.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(10) # Let new settings take affect.
sensor.set_whitebal(False) # turn this off.
#关闭白平衡。白平衡是默认开启的，在颜色识别中，需要关闭白平衡。
clock = time.clock() # Tracks FPS.

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # Take a picture and return the image.

    blobs = img.find_blobs([green_threshold])
    #find_blobs(thresholds, invert=False, roi=Auto),thresholds为颜色阈值，
    #是一个元组，需要用括号［ ］括起来。invert=1,反转颜色阈值，invert=False默认
    #不反转。roi设置颜色识别的视野区域，roi是一个元组， roi = (x, y, w, h)，代表
    #从左上顶点(x,y)开始的宽为w高为h的矩形区域，roi不设置的话默认为整个图像视野。
    #这个函数返回一个列表，[0]代表识别到的目标颜色区域左上顶点的x坐标，［1］代表
    #左上顶点y坐标，［2］代表目标区域的宽，［3］代表目标区域的高，［4］代表目标
    #区域像素点的个数，［5］代表目标区域的中心点x坐标，［6］代表目标区域中心点y坐标，
    #［7］代表目标颜色区域的旋转角度（是弧度值，浮点型，列表其他元素是整型），
    #［8］代表与此目标区域交叉的目标个数，［9］代表颜色的编号（它可以用来分辨这个
    #区域是用哪个颜色阈值threshold识别出来的）。
    if blobs:
    #如果找到了目标颜色
        for b in blobs:
        #迭代找到的目标颜色区域
            # Draw a rect around the blob.
            img.draw_rectangle(b[0:4]) # rect
            #用矩形标记出目标颜色区域
            img.draw_cross(b[5], b[6]) # cx, cy
            #在目标颜色区域的中心画十字形标记
    print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while
    # connected to your computer. The FPS should increase once disconnected.

#确定目标是否存在（True:存在）
def checkTargetExists(rects):
    targetIdx = checkTargetOnce(rects);
    if targetIdx == -1:
        noneTargetLastCount++;
    else
        noneTargetLastCount = 0;
    return noneTargetLastCount < NONE_TARGET_COUNT_MIN


#查找当前画面中，有效面积最大的矩形索引
def checkTargetOnce(rects):
    rectIdx = 0
    tempAreaMax = 0
    tempAreaMaxIdx = -1
    for item in rects:
        tempArea = item[2] * item[3];
        if tempArea > AREA_PIXEL_MIN and tempArea > tempAreaMax :
            tempAreaMax = tempArea
            tempAreaMaxIdx= rectIdx
        rectIdx++;
    return tempAreaMaxIdx

#根据偏离中心位置距离，将成像画面转换为左右转动的速度
def moveLeftRight(xCenter):
    deltaX = xCenter - (IMAGE_WIDTH / 2);
    turnDeltaSum += deltaX;
    turnVelocity = KP_TURN*deltaX + KI_TURN*turnDeltaSum + KD_TURN*(deltaX - turnDeltaLast)/DELTA_TIME_CHECK;
    turnDeltaLast=deltaX;

#根据跟标准面积的差距，将画面转换为前后速度
def moveForwardBackward(area):
    deltaX = area - IMAGE_AREA_SIZE_DEFAULT;
    moveDeltaSum += deltaX;
    moveVelocity = KP_MOVE*deltaX + KI_MOVE*moveDeltaSum + KD_MOVE*(deltaX - moveDeltaLast)/DELTA_TIME_CHECK;
    moveDeltaLast = deltaX;
