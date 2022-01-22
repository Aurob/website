import urllib.request as urlr
import json, sys, os
from PIL import Image
import PIL
from PIL import ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True

if __name__ == "__main__":
    if len(sys.argv)>1:
        config_name = sys.argv[1]    
        print(config_name)
        config_path = "/var/www/html/test/resources/scenes/"+config_name+".json"
        config = open(config_path).read()
        json_config = json.loads(config)
        i=-1
        file_names = []
        delete_list = []
        for scene in json_config["Scenes"]:
            for entity in scene["entities"]:
                for component in entity["components"]:
                    if component["type"] == 8:
                        try:
                            file_name = "/var/www/html/test/resources/images/"+str(file_names.index(component["url"]))+".png"
                            print(file_name)
                            component["file_name"] = file_name
                            
                        except:
                            try:
                                print(component["url"])
                                file_names.append(component["url"])
                                file_name = "/var/www/html/test/resources/images/"+str(len(file_names)-1)+".png"
                                # file_local = component["url"].replace("https://robauis.me/test/resources/images/", "")
                                # file_exist = False
                                # if file_local in os.listdir("/var/www/html/test/resources/images/"):
                                #     file_exist = True
                                #     file_name = "/var/www/html/test/resources/images/"+file_local
                                # else:   
                                    
                                urlr.urlretrieve(component["url"], file_name)
                                png_conv = Image.open(file_name)
                                
                                png_conv = png_conv.rotate(180, PIL.Image.NEAREST, expand = 1)

                                png_conv.save(file_name)
                                # if not(file_exist):
                                delete_list.append(file_name)
                                component["file_name"] = file_name
                            except Exception as e:
                                print("skipping image", e)
                            

        open(config_path, "w").write(str(json_config).replace("'",'"'))
        open("/var/www/html/test/resources/delete.txt", "w").write("\n".join(delete_list))
    else:
        print("config file name required")