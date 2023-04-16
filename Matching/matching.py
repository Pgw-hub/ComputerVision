import cv2
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

MODE = "3D"

img_paths = ['../data/matching/pinhole/1.jpg','../data/matching/pinhole/2.jpg','../data/matching/pinhole/3.jpg','../data/matching/pinhole/4.jpg','../data/matching/pinhole/5.jpg','../data/matching/pinhole/6.jpg',]

# SIFT 특징점 추출기 생성
sift = cv2.SIFT_create()
K = np.array([[640, 0, 320], [0, 640, 240], [0, 0, 1]])  # 내부 파라미터 행렬

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')

for i in range(len(img_paths)-1) :
    img1 = cv2.imread(img_paths[i], cv2.IMREAD_GRAYSCALE)
    img2 = cv2.imread(img_paths[i + 1], cv2.IMREAD_GRAYSCALE)

    # 특징점 검출 및 디스크립터 계산
    kp1, des1 = sift.detectAndCompute(img1, None)
    kp2, des2 = sift.detectAndCompute(img2, None)

    # 매칭 수행
    FLANN_INDEX_KDTREE = 1
    index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
    search_params = dict(checks=50)
    flann = cv2.FlannBasedMatcher(index_params, search_params)
    matches = flann.knnMatch(des1, des2, k=2)

    # 좋은 매칭점 추출
    good_matches = []
    for m, n in matches:
        if m.distance < 0.7 * n.distance:
            good_matches.append(m)

    # 매칭 결과 출력
    img_matches = cv2.drawMatches(img1, kp1, img2, kp2, good_matches, None, flags=cv2.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
    cv2.imshow('Matches', img_matches)
    cv2.waitKey()

    # 카메라 포즈 추정
    src_pts = np.float32([kp1[m.queryIdx].pt for m in good_matches]).reshape(-1, 1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good_matches]).reshape(-1, 1, 2)
  
    E, mask = cv2.findEssentialMat(src_pts, dst_pts, K)
    _, R, t, mask = cv2.recoverPose(E, src_pts, dst_pts, K)
    
    print("Rotation:")
    print(R)
    print("Translation:")
    print(t)