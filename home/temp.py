import csv
import json
import random
import cv2
import numpy as np
import urllib.request as urlr
import ssl
import motion
import time
ssl._create_default_https_context = ssl._create_unverified_context

global showim

file = "artists.json"

db = json.loads(open(file).read())
update = False
showim = None
orig = None
step=0
msize = 10

def getImg(url):
    imread = urlr.urlopen(url)
    image = np.asarray(bytearray(imread.read()), dtype="uint8")
    image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    return image

def onMouse(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
       last_draw = time.time()
       # draw circle here (etc...)
       print('x = %d, y = %d'%(x, y))
       #
       y = find_div(y, square_size)
    x = find_div(x, square_size)
       frames = [param.copy() for i in range(4)]
        for _y in range(0, y, f):
            for _x in range(0, x, f):
                sliced = img[_y:_y+f, _x:_x+f]
                for i in range(4):
                    sim = makeart(sliced, i)
                    frames[i][_y:_y+f, _x:_x+f] = cv2.cvtColor(sim, cv2.COLOR_BGR2RGB)
       cropped = param[y-msize:y+msize, x-msize:x+msize]
       print(len(param), step)
       showim[y-msize:y+msize, x-msize:x+msize] = motion.makeart(cropped, step)

       

while True:
    if not(update):
        showim = getImg("https://cdni.pornpics.com/1280/7/219/55703098/55703098_160_02ff.jpg")
        scale_percent = 90 # percent of original size
        width = int(showim.shape[1] * scale_percent / 100)
        height = int(showim.shape[0] * scale_percent / 100)
        dim = (width, height)
        
        showim = cv2.resize(showim, dim, interpolation = cv2.INTER_AREA)
        orig = showim.copy()
        update = True
        ''''
        a = random.randint(0, len(db)-1)
        w = random.randint(0, len(db[a]["works"])-1)
        url = db[a]["works"][w]["image_url"]

        if not(url.endswith(".jpg")):
            url = url+"/"+db[a]["works"][w]["wga_url"].split(".html")[0].split("/")[-1:][0]+".jpg"

        img = getImg(url)
        if not(isinstance(img, type(None))):
            showim = img.copy()
            scale_percent = 60 # percent of original size
            width = int(img.shape[1] * scale_percent / 100)
            height = int(img.shape[0] * scale_percent / 100)
            dim = (width, height)
            title = db[a]["works"][w]["title"]
            artist = db[a]["artist"].split(",")[0]
            filename = (title+"_"+artist).replace(" ", "-")
            # resize image
            y = motion.find_div(height, 4)
            x = motion.find_div(width, 4)
            showim = cv2.resize(showim, dim, interpolation = cv2.INTER_AREA)
            orig = showim.copy()
            step=1
            update = True
            '''
            
        #else:
        #    print(db[a]["works"][w])
    elif not(isinstance(showim, type(None))):
        cv2.imshow("", showim)
            
        cv2.setMouseCallback("", onMouse, orig)
        key = cv2.waitKey(1) & 0xFF

        if key == ord("s"):
            cv2.imwrite("./art/"+filename+".png", showim)
            print("Saved ", filename)
        elif key == ord("m"):
            print(step)
            showim = motion.makeart(showim, step)
            step += 1
            if step > 3:
                step = 1
        elif key == ord("r"):
            showim = orig.copy()
        elif key == ord("q"):
            update = False
        elif key == ord("."):
            msize += 1
            print(msize)
        elif key == ord(","):
            msize -=1
            print(msize)
        elif key == ord("b"):
            step += 1
            if step > 3:
                step = 1
            print(step)
        elif key == ord("v"):
            step -= 1
            if step < 0:
                step = 3
            print(step)
        elif key != 255:
            print(key)

cv2.destroyAllWindows()
'''
artist_artlist = []
artists = []
with open('catalog.csv', newline='') as file:
    reader = csv.reader(file, delimiter=',', quotechar='"')
    skip = 0
    artist_count = 0
    for row in reader:
        if skip > 0 :
            artist = row[0]
            #artist_keys = [list(i.keys())[0] for i in artist_artlist]

            if not(artist in artists):
                artists.append(artist)
                artist_artlist.append({ 'artist' : artist, 'works' : [], 'active_range' : row[1] })
                
            artist_artlist[artists.index(artist)]['works'].append({
                'title' : row[2],
                'year' : row[3],
                'medium' : row[4],
                'location' : row[5],
                'wga_url' : row[6],
                'image_url' : '/'.join(row[6].split("/")[:3] + \
                                       ['art'] + row[6].split("/")[4:6] + \
                                       [row[6].split("/")[6].replace("html", "jpg")]),
                'type' : row[7],
                'genre' : row[8],
                'country' : row[9],
                'date_range' : row[10]
            })
        skip+=1        
'''
