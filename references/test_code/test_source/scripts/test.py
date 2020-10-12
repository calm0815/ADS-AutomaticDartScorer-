# -*- coding: utf-8 -*-
import cv2

# トラックバーの値を変更する度にRGBを出力する
def changeColor(val):
    r_min = cv2.getTrackbarPos("R_min", "img")
    r_max = cv2.getTrackbarPos("R_max", "img")
    g_min = cv2.getTrackbarPos("G_min", "img")
    g_max = cv2.getTrackbarPos("G_max", "img")
    b_min = cv2.getTrackbarPos("B_min", "img")
    b_max = cv2.getTrackbarPos("B_max", "img")
    mask_image = cv2.inRange(img, (b_min, g_min, r_min), (b_max, g_max, r_max)) # BGR画像なのでタプルもBGR並び
    # (X)ウィンドウに表示
    cv2.namedWindow("img", cv2.WINDOW_NORMAL)
    cv2.imshow("img", mask_image)

# 画像の読み込み
img = cv2.imread("../../resources/capture_l_plane.png", 1)
img = cv2.resize(img , (int(img.shape[1]*0.5), int(img.shape[0]*0.5)))
# ウィンドウのサイズを変更可能にする
cv2.namedWindow("img", cv2.WINDOW_NORMAL)

# トラックバーの生成
cv2.createTrackbar("R_min", "img", 0, 255, changeColor)
cv2.createTrackbar("R_max", "img", 0, 255, changeColor)
cv2.createTrackbar("G_min", "img", 0, 255, changeColor)
cv2.createTrackbar("G_max", "img", 0, 255, changeColor)
cv2.createTrackbar("B_min", "img", 0, 255, changeColor)
cv2.createTrackbar("B_max", "img", 0, 255, changeColor)

# 「Q」が押されるまで画像を表示する
while (True):
    # cv2.imshow("img", mask_image)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cv2.destroyAllWindows()