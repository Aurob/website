import subprocess

cams = open("cams.json").read()
cams = eval(cams)
for cam in cams:
    cam = cams[cam]
    # subprocess.call(["curl", ])
    print(cam["base"]+str(1)+cam["end"])