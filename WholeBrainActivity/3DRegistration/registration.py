# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 17:41:11 2017

@author: varshinig
"""


import tifffile
import skimage
import cv2
from skimage.external.tifffile import imread, TiffFile, imshow
from skimage.color import rgb2gray
from skimage import io, transform, img_as_ubyte
from skimage.color import rgb2gray
import numpy as np
import matplotlib.pyplot as plt
import os

temp = imread('C:/Users/varshinig/Dropbox/Registration/template/template.tif')
tgt = imread('C:/Users/varshinig/Dropbox/Registration/control_001/Substack.tif')

## function to register target images to template

def  registerImage(template_image, target_image):
    
    #template = img_as_ubyte(template_image)
    #target = img_as_ubyte(target_image)
    
    
    template = cv2.cvtColor(template_image,cv2.COLOR_RGB2BGR)
    target = cv2.cvtColor(target_image,cv2.COLOR_RGB2BGR)
    # Read the images to be aligned
    #template =  cv2.imread(template_image);
    #target =  cv2.imread(target_image);
 
    # Convert images to grayscale
    template_gray = cv2.cvtColor(template,cv2.COLOR_BGR2GRAY)
    target_gray = cv2.cvtColor(target,cv2.COLOR_BGR2GRAY)
 
    # Find size of image1
    sz = template.shape
 
    # Define the motion model
    warp_mode = cv2.MOTION_TRANSLATION
 
    # Define 2x3 or 3x3 matrices and initialize the matrix to identity
    if warp_mode == cv2.MOTION_HOMOGRAPHY :
        warp_matrix = np.eye(3, 3, dtype=np.float32)
    else :
        warp_matrix = np.eye(2, 3, dtype=np.float32)
 
    # Specify the number of iterations.
    number_of_iterations = 5000;
 
    # Specify the threshold of the increment
    # in the correlation coefficient between two iterations
    termination_eps = 1e-10;
 
    # Define termination criteria
    criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, number_of_iterations,  termination_eps)
 
    # Run the ECC algorithm. The results are stored in warp_matrix.
    (cc, warp_matrix) = cv2.findTransformECC (template_gray,target_gray,warp_matrix, warp_mode, criteria)
 
    if warp_mode == cv2.MOTION_HOMOGRAPHY :
        # Use warpPerspective for Homography 
        target_aligned = cv2.warpPerspective (target, warp_matrix, (sz[1],sz[0]), flags=cv2.INTER_LINEAR + cv2.WARP_INVERSE_MAP)
    else :
        # Use warpAffine for Translation, Euclidean and Affine
        target_aligned = cv2.warpAffine(target, warp_matrix, (sz[1],sz[0]), flags=cv2.INTER_LINEAR + cv2.WARP_INVERSE_MAP);
 
    # Show final results
    #cv2.imshow("Image 1", template)
    #cv2.imshow("Image 2", target)
    #cv2.imshow("Aligned Image 2", target_aligned)
    #cv2.waitKey(0)
    
    return target_aligned

#path1 = "C:/Users/varshinig/Dropbox/Registration/template/temp_5.tif";
#path2 = "C:/Users/varshinig/Dropbox/Registration/control_001/slice5.tif";
#target_aligned = registerImage(path1, path2)
#cv2.imshow("Aligned Image 2", target_aligned)
#cv2.waitKey(0)

## 
for i in range(0,temp.shape[0]-1):
    aligned_image[i] = registerImage(temp[i], tgt[i])
