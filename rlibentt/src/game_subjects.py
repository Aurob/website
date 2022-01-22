import sys, json

def makeSubject(name):
    subject = {}
    subject["subject"] = name
    subject["id"] = name
    subject["title"] = name
    subject["sub_title"] = "View this scene in the editor: <a target=\"_blank\" href=\"/scenebuilder/?scene="+name+"\">"+name+"</a>"
    subject["content"] = "<iframe src=\"build/"+name+"/index.html\"></iframe>"
    subject["width"] = "70%"
    subject["default"] = "true"
    return subject

if __name__ == "__main__":
    if len(sys.argv) > 1:
        game_name = sys.argv[1]
        subjects = json.loads(open("/var/www/html/test/subjects.json").read())
        subject = makeSubject(game_name)
        subjects["data"][game_name] = subject

        open("/var/www/html/test/subjects.json", "w").write(json.dumps(subjects))