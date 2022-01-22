from flask import Flask
from flask import request
import os
import secrets
import json

app = Flask(__name__)

saved = {"abcd1234":"This is a test note"}
for i in os.listdir("documents"):
    if i.endswith(".json"):
        saved[i]= open("documents/"+i, "r").read()

@app.route("/api/notes")
def home():
    print(request.args)
    if "load" in request.args:
        load_id = request.args.get("load")
        if load_id in saved:
            print(saved[load_id])
            contents = json.loads(saved[load_id])
            out = []
            for content in contents:
                out.append(content["insert"])

            print(out)
            return {
                "response":"1",
                "subject":"This note",
                "id":"thisnote",
                "title":load_id,
                "sub_title":"test load",
                "content":'\n'.join(out)
            }
        return {"response": "0"}
    
    if "save" in request.args:
        if "data" in request.args:
            print(request.args["data"])
        save_id = secrets.token_urlsafe(16)
        saved[save_id] = "This is not a test"
        return {"save_id" : save_id}


if __name__ == "__main__":
    app.run()

