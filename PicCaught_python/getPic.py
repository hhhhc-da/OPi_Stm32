#coding:utf-8
import cv2, os, time
import numpy as np

camera_path = '/dev/video11'

initial_focus = 30
weight, height = 3, 4
lr = 4

cap = cv2.VideoCapture(camera_path)

def init():
    if(cap.isOpened()):
        cap.set(weight, 1024)
        cap.set(height, 768) 
        
        print('Camera({}) initialze OK'.format(camera_path))
    else:
        print('Camera({}) initialze failed'.format(camera_path))
                
def destroy():
    if(cap.isOpened()):
        cap.release()
        try:
            cv2.destroyAllWindows()
        except Exception as e:
            print(e)  
        print('Camera({}) detroy OK'.format(camera_path))   
    else:
        print('Camera({}) detroy failed'.format(camera_path))
        
def culculate(image):
    # variance: gradient amplitude
    # result more large, commonly more fittable
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gradient_x = cv2.Sobel(gray, cv2.CV_64F, 1, 0, ksize=3)
    gradient_y = cv2.Sobel(gray, cv2.CV_64F, 0, 1, ksize=3)
    gradient_magnitude = np.sqrt(gradient_x**2 + gradient_y**2)
    result = np.var(gradient_magnitude)
    return result

def focus():
    if(cap.isOpened()):
        global initial_focus, lr
        lr = 4
        
        # initialize
        cap.set(cv2.CAP_PROP_FOCUS, initial_focus)
        ret, frame = cap.read()
            
        initial_focus_measure = culculate(frame)
        current_focus = initial_focus + lr
        
        recent_data = [0,0,0,0,0,0,0,0,0,0]
        i = 0
            
        while True:
            ret, frame = cap.read()

            current_focus_measure = culculate(frame)
            print('Focus: {:.1f}, Focus Measure: {:.4f}'.format(current_focus, current_focus_measure))
            
            if abs(current_focus_measure - initial_focus_measure) < 0.05:
                print('Auto focus OK!')
                break
        
            if current_focus_measure > initial_focus_measure:
                focus_incr = lr
                recent_data[i%10] = 1
            else:
                focus_incr = 0-lr
                recent_data[i%10] = 0
                
            # update initial_data
            initial_focus_measure = current_focus_measure
            initial_focus = current_focus
                
            if(i > 10):
                # decrease learning rate
                if recent_data[i%10] != recent_data[(i-1)%10] and recent_data[(i-3)%10] == recent_data[(i-1)%10] and recent_data[(i-2)%10] == recent_data[i%10]:
                    lr = lr/4
            
                
            current_focus = initial_focus + focus_incr
            
            cap.set(cv2.CAP_PROP_FOCUS, current_focus)
    else:
        print('Camera({}) focus failed'.format(camera_path))
        
    print('Camera({}) focus OK'.format(camera_path))

def downloadPic():
    if(cap.isOpened()):
        ret, frame = cap.read()
        
        cv2.imwrite('./test.jpg', frame)
        os.system('chmod -R 777 test.jpg && xdg-open test.jpg')
        
    else:
        print('Camera({}) download failed'.format(camera_path))

if __name__ == '__main__':
    
    _start = time.time()

    init()
    focus()
    downloadPic()
    destroy()
    
    _end = time.time()
    _time = _end - _start
    
    print('Picture save OK\nTime: '+ str(_time) +'\nexit program.')
    