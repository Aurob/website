$(document).ready(function() {
    loadEvents();
});

function loadEvents() {
    $("#canvas").on("mousedown", function(e) {
        if(e.which == 1) {
            mousedown = true;
            let mouse_pos = getMousePos(canvas, e);
            
            mousex = mouse_pos.x;
            mousey = mouse_pos.y;
            selected = checkObjBounds(mousex, mousey);
            if(!selected) selected = false;
            undo_history.push(JSON.parse(JSON.stringify(scenes)));
        }
        e.preventDefault();
    });
    
    $("#canvas").on("mouseup", function(e) {
        mousedown = false;
        // selected = false;
    
        updateMenu();
    });
    
    $("#canvas").on("mousemove", function(e) {
        let mouse_pos = getMousePos(canvas, e);
        mousex = mouse_pos.x;
        mousey = mouse_pos.y;
        if(mousedown) {
            //Dragging entity
            if(selected) {
                let entity = selected;
                let order = getOrder(entity.id);
                let position = getValue(scenes[order[0]].entities[order[1]].components, "Position");
                
                position.x = (Math.ceil(mousex)) - (Math.ceil(mousex) % 5);//shape.w/2;
                position.z = (Math.ceil(mousey)) - (Math.ceil(mousey) % 5); // - shape.l/2;
            }
        }
        
        
    });
    
    var ctrl = false;
    var shift = false;
    $(window).on("keydown", function(e) {
        // console.log(e.key);
    
        //Misc key events
        switch(e.key) {
            case "Control" :
                ctrl = true;
                break;
            case "Shift" :
                shift = true;
                break;
            case "z" :
                if(ctrl) {
                    undo();
                }
                break;
    
            case "y" :
                if(ctrl) {
                    redo();
                }
                break;
            
            case "Enter" :
                if($("#new_obj").css("display") != "none") {
                    $("#add_obj").click();
                }
                break;
            case "l" :
                show_labels = !show_labels;
                break;
    
            default:
                break;
        }
        
        //Selected Entity key events
        if(selected) {
            let order = getOrder(selected.id);
            // console.log()
            let shape = getValue(scenes[order[0]].entities[order[1]].components, "Shape");
            let position = getValue(scenes[order[0]].entities[order[1]].components, "Position");
            switch(e.key) {
                case "ArrowUp":
                case "w":
                    position.z -=  5;
                    break;
                case "ArrowDown":
                case "s":
                    position.z += 5;
                    break;
                        
                case "ArrowLeft":
                case "a":
                    position.x -= 5;
                    break;
                                
                case "ArrowRight":
                case "d":
                    position.x += 5;
                    break;
                
                case "W" : {
                    shape.l -= 5;
                    position.z -= 2.5;
                    break;
                }
                
                case "S" : {
                    shape.l += 5;
                    break;
                }
    
                case "A" : {
                    shape.w -= 5;
                    break;
                }
    
                case "D" : {
                    shape.w += 5;
                    break;
                }
    
                case "_" : {
                    position.y -= 5;
                    break;
                }
    
                case "+" : {
                    position.y += 5;
                    break;
                }
    
                case "-" : 
                    shape.h -= 5;
                    break;
    
                case "=" : 
                    shape.h += 5;
                    break;
                
                //TODO make a function for this!
                case "&":
                    //Clicks the duplicate button for this entity
                    $("#col_entity_"+selected.id+"_"+scenes[order[0]].id).find(".fa-retweet").click();
                    break;
    
                default:
                    break;        
            }
        }
    });
    
    $(window).on("keyup", function(e) {
        switch(e.key) {
            case "Control" :
                ctrl = false;
                break;
            default:
                break
        }
    });
    
    $("#import").click(function(e) {
        $("#importModal").modal("toggle");
    });

    $("#import_btn").click(function(e) {
        loadScene($("#import_text").val());
    });
    
    $("#export").click(function(e) {
        let scene_list = [];
        let scene_copy = JSON.parse(JSON.stringify(scenes, null, "\t"));
        
        let textures = [];
        let models = [];
        scene_copy.forEach(scene => {
            scene.name = "test";
            scene.width = 1000;
            scene.height = 1000;
            // scene.grid = true;
            for(let e = 0; e < scene.entities.length; e++) {
                let entity = scene.entities[e];
                for(let c = 0; c < entity.components.length; c++) {
                    switch(scene.entities[e].components[c].type) {
                        case "Position":
                            scene.entities[e].components[c].type = 0;
                            break;
                        case "Shape":
                            scene.entities[e].components[c].type = 1;
                            break;
                        case "Interaction":
                            scene.entities[e].components[c].type = 3;
                            break;
                        case "Teleport":
                            scene.entities[e].components[c].type = 4;
                            break;
                        case "Color":
                            scene.entities[e].components[c].type = 7;
                            break;
                        case "Texture":
                            if(textures.indexOf(scene.entities[e].components[c].url) == -1) {
                                textures.push(scene.entities[e].components[c].url);
                                scene.entities[e].components[c].id = textures.length-1;
                            }
                            else scene.entities[e].components[c].id = textures.indexOf(scene.entities[e].components[c].url);
                            scene.entities[e].components[c].type = 8;
                            // textures.push(scene.entities[e].components[c].url);
                            break;
                        case "Model":
                            console.log(c);
                            if(models.indexOf(scene.entities[e].components[c].file_name) == -1) {
                                models.push(scene.entities[e].components[c].file_name);
                                scene.entities[e].components[c].id = models.length-1;
                            }
                            else scene.entities[e].components[c].id = models.indexOf(scene.entities[e].components[c].file_name);
                            scene.entities[e].components[c].type = 9;
                            break;
                        default:
                            break;
                    }
                }
            }
            scene_list.push(scene);
        });
        
        scene_json = {
            "Scenes": scene_list,
            "tex_count": textures.length,
            "model_count": models.length
        };
        scene_json = JSON.stringify(scene_json);
        navigator.clipboard.writeText(scene_json);
        $("#export_body").html(scene_json);
        $("#show_export").modal("toggle");
    });
    
    var menu_shown = false;
    $("#entity_menu").click(function(e) {
        if(menu_shown) {
            $("#menucol").css("display", "none");
            menu_shown = false;
        }
        else {
            $("#menucol").css("display", "block");
            menu_shown = true;
        }
        return false;
    });
    
    $("#undo").on("mousedown", function(e) {
        undo();
    });
    $("#redo").on("mousedown", function(e) {
        redo();
    });
}


function checkObjBounds(x, y) {
    for(let s = 0; s < scenes.length; s++) {
        let scene = JSON.parse(JSON.stringify(scenes[s]));

        scene.entities.sort(function(e1, e2) {                
            let e1pos = getValue(e1.components, "Position");
            let e1shape = getValue(e1.components, "Shape");
            let e2pos = getValue(e2.components, "Position");
            let e2shape = getValue(e2.components, "Shape");

            if(e1shape == undefined || e1pos == undefined) return 1;
            else if(e2shape == undefined || e2pos == undefined) return -1;
            else if(e1pos.y + (e1shape.h/2) > e2pos.y + (e2shape.h/2)) return -1;
            else if(e1pos.y + (e1shape.h/2) < e2pos.y + (e2shape.h/2)) return 1;
            return 0;
        });
        for(let e = 0; e < scene.entities.length; e++) {
            let entity = scene.entities[e];
            let position = getValue(entity.components, "Position");
            let shape = getValue(entity.components, "Shape");
            
            if(position) {
                
                //Allow moving of point entities (positions but no shapes)
                if(!shape) shape = {"w":10, "l":10}

                if(x + (shape.w/2) > position.x && x + (shape.w/2) < position.x + shape.w) {
                    if(y + (shape.l/2) > position.z && y + (shape.l/2) < position.z + shape.l) {
                        return entity;
                    }
                }
            }
        }
    }
    return false;
}

function getMousePos(canvas, evt) {
    var rect = canvas.getBoundingClientRect();
    return {
        x: (evt.clientX - rect.left) - (width/2),
        y: (evt.clientY - rect.top) - (length/2)
    };
}