var shown = [];
var hidden_scenes = [];
function updateMenu() {
    let shown = $(".fa-sort-up").parent().parent().parent();
    let shown_ids = [];
    for(let s = 0; s < shown.length; s++) {
        if(shown[s].id) {
            console.log(shown[s].id)
            shown_ids.push(shown[s].id);
        }
    }

    $("#menucol").html("");
    let new_root = newMenu(0, "root");
    $("#menucol").append(new_root);
    for(let s = 0; s < scenes.length; s++) {
        let scene = scenes[s];
        let sid = scene.id; //scenes.indexOf(scene);
        // let scol = newCol();
        // scol.append(newMenu(0, "scene", 0));
        let new_scene = newMenu(sid, "scene", 0, [s]);
        // if(shown.indexOf([undefined, sid])>-1) new_scene.css("display", "block");
        // else new_scene.css("display", "none");
        // if(scene.shown) new_scene.css("display", "block");
        // else new_scene.css("display", "none");
    
        $("#col_root_0").append(new_scene);
        for(let e = 0; e < scene.entities.length; e++) {
            let entity = scene.entities[e];
            let eid = entity.id;//scene.entities.indexOf(entity);
            // let ecol = newCol();
            // ecol.append(newMenu(eid, "entity", sid));
            let new_entity = newMenu(eid, "entity", sid, [s, e]);
            // if(shown.indexOf([sid, eid])>-1) new_enitiy.css("display", "block");
            // else new_enitiy.css("display", "none");
            // if(entity.shown) new_entity.css("display", "block");
            // else new_entity.css("display", "none");
        
            $("#col_scene_"+sid).append(new_entity);
            for(let c = 0; c < entity.components.length; c++) {
                let component = entity.components[c];
                let cid = component.id;//entity.components.indexOf(component);
                let new_component = newMenu(cid, "component", eid, [s, e, c]);
                
                // if(component.shown) new_component.css("display", "block");
                // else new_component.css("display", "none");
                $("#col_entity_"+eid+"_"+sid).append(new_component);
                for(let v = 0; v < Object.keys(component).length; v++) {
                    let value = Object.keys(component)[v];
                    let vid = v;//Object.keys(component).indexOf(value);
                    let new_value = newMenu(vid, "value", cid, [s, e, c, v]);
                    $("#col_component_"+cid+"_"+eid).append(new_value);
                }
            }
        }    
    }

    //uncollapse each entity that was previously uncollapsed

    shown_ids.forEach(id => {
        let el = $("#"+id);
        el.children().css("display", "none");
        $($($(el.children()[0]).children()[0]).children()[0]).removeClass("fa-sort-down");
        $($($(el.children()[0]).children()[0]).children()[0]).addClass("fa-sort-up");
    });
}

function newMenu(id, label, root_id, order) {
        
    let next;
    let item;
    switch(label.toLowerCase()) {
        case "root":
            next = "scene";
            break;
        case "scene":
            item = scenes[order[0]];
            next = "entity";
            break;
        case "entity":
            item = scenes[order[0]].entities[order[1]];
            next = "component";
            break;
        case "component":
            item = scenes[order[0]].entities[order[1]].components[order[2]];
            next = "value";
            break;
        case "value":
            next = "none";
        default:
            break;
    }

    let container;
    if(root_id==-1) container = $("<div class='container-fluid'></div>");
    
    let row = $("<div class='row g-0'></div>");
    row.attr("name", id);
    let col = newCol();

    let _id = label + "_" + id;
    if(root_id) _id += "_" + root_id;

    if(root_id==-1) container.attr("id", _id);
    row.attr("id", "row_"+_id);
    col.attr("id", "col_"+_id);
    

    //Button to add new child menu
    let new_btn = $("<i class='fa fa-plus-square' title='add a new "+next+" menu item'></i>");
    let sid, eid, cid;
    new_btn.click(function() {
        if(next == "scene") {
            let next_id = makeid(); //scenes.length;
            scenes.push(newScene(next_id));
            updateMenu();
        }
        else if(next == "entity") {
            let next_id = makeid(); //scenes[id].entities.length;
            scenes[order[0]].entities.push(newEntity(next_id));
            updateMenu();
        }
        else if(next == "component") {
            
            // let next_id = scenes[root_id].entities[id].components.length;
            // //add a component picker here
            // scenes[root_id].entities[id].components.push());
            console.log(newComponent(order));
            updateMenu();
        }
        else if(next == "value") {
            updateMenu();
        }
    });

    //Button to remove child menu
    let del_btn = $("<i class='fa fa-times-circle' title='remove this "+label+"'></i>");
    del_btn.click(function() {
        if(label == "scene") {
            scenes.splice(order[0],1);
            updateMenu();
        }
        else if(label == "entity") {
            scenes[order[0]].entities.splice(order[1],1);
            updateMenu();
        }
        else if(label == "component") {
            sid = row.parent().closest(".row").attr("name");
            scenes[order[0]].entities[order[1]].components.splice(order[2],1);
            updateMenu();
        }
    });

    //Button to copy scenes and entities
    let copy_btn = $("<i class='fa fa-retweet' title='clone "+label+" menu item'></i>");
    copy_btn.click(function() {
        let copy;
        if(label == "scene") {
            copy = JSON.parse(JSON.stringify(scenes[order[0]]));
            copy.id = makeid(); //scenes.length;
            console.log(copy)
            copy.entities.forEach(entity =>{
                entity.id = makeid();
                entity.components.forEach(component => {
                    component.id = makeid();
                });
            });
            console.log(copy)
            scenes.push(copy);
            updateMenu();
        }
        else if(label == "entity") {
            copy = JSON.parse(JSON.stringify(scenes[order[0]].entities[order[1]]));
            copy.id = makeid(); //scenes[order[0]].entities.length;
            scenes[order[0]].entities.push(copy);
            updateMenu();
        }

    });

    //Button to show/hide body
    let collapse_btn = $("<i class='collapse-btn fa fa-sort-down' title='hide/show all descendant "+next+" menu items'></i>");            
    collapse_btn.click(function() {
        if($(this).hasClass("fa-sort-down")) {
            $("#col_"+_id).children().css("display", "none");
            $(this).removeClass("fa-sort-down");
            $(this).addClass("fa-sort-up");
            // $("#col_"+_id).removeClass("shown");
            // item.shown = false;
            $("#col_"+_id).attr("shown", false);
        }
        else {
            $("#col_"+_id).children().css("display", "flex")
            $(this).removeClass("fa-sort-up");
            $(this).addClass("fa-sort-down");
            // $("#col_"+_id).addClass("shown");
            $("#col_"+_id).attr("shown", true);
            // item.shown = true;
        }                
    });

    // let hide_btn;
    // if(label == "scene") {
    //     //Button to show/hide entire scene
    //     hide_btn = $("<i class='collapse-btn fa fa-eye-slash' title='hide/show this scene'></i>");            
    //     hide_btn.click(function() {
    //         if($(this).hasClass("fa-eye-slash")) {
    //             $(this).removeClass("fa-eye-slash");
    //             $(this).addClass("fa-eye");
    //             hidden_scenes.push(order[0]);
    //         }
    //         else {
    //             $(this).removeClass("fa-eye");
    //             $(this).addClass("fa-eye-slash");
    //             hidden_scenes.splice(hidden_scenes.indexOf(order[0]), 1);
    //         }                
    //     });
    // }

    //Create Menu Item label and action buttons
    let flex = newFlex();
    let menu_label = newFlexItem();
    let menu_show = newFlexItem();
    let menu_new = newFlexItem();
    let menu_del = newFlexItem();
    let menu_copy = newFlexItem();
    let menu_scene_hide = newFlexItem();

    if(label == "value") {
        let field = Object.keys(scenes[order[0]].entities[order[1]].components[order[2]])[order[3]];
        let input = $("<input type='text' name='"+field+"' id='"+_id+"'>");
        input.on("change", function(evt) {
            let val = evt.currentTarget.value;
            
            if(!isNaN(val)) val = parseFloat(evt.currentTarget.value);
            if(evt.target.name == "type") val = Components[val]
            if(evt.target.name == "file_name") {
                if(textures.indexOf(val) < 0) {
                    textures.push(val);
                }
            }
            scenes[order[0]].entities[order[1]].components[order[2]][evt.target.name] = val;
            updateMenu();
        });
        input.val(scenes[order[0]].entities[order[1]].components[order[2]][field])
        flex.append(menu_label.append("<i>"+field+"</i>", input));
    }
    else flex.append(menu_show.append(collapse_btn), menu_label.append("<p>"+_id+"</p>"));
    
    if(label != "component" && label != "value") {
        menu_new.append(new_btn);
    }
    flex.append(menu_new);
    if(label != "root" && label != "value") {
        menu_del.append(del_btn);
        flex.append(menu_del);
    }
    if(label == "scene" || label == "entity") {
        menu_copy.append(copy_btn);
        flex.append(menu_copy);

        // if(label == "scene") {
        //     menu_scene_hide.append(hide_btn);
        //     flex.append(menu_scene_hide);
        // }
    }
    col.append(flex);
    row.append(col);

    return row;
}


function newScene(id) {
    let s = {
        "id" : id,
        "entities" : [

        ]
    }
    return s;
}

function newEntity(id) {
    let e = {
        "id" : id,
        "components" : [
        ]
    }

    return e;
}
    
function newComponent(order) {
    //Trigger component window
    let cancel = false;
    $("#new_obj").modal("toggle");
    $("#add_obj").click(function(e) {
        
        let entity =  scenes[order[0]].entities[order[1]];
        let obj_form = $("#component_fields")[0];

        let fields = $(obj_form).children();
        let component_type = $("#component_list").val();

        if(!getValue(entity.components, component_type)) {
            let component = components[component_type];
            let new_c = {"id":makeid(), "type":component_type};
            component[0].concat(component[1]).forEach(key => {
                new_c[key] = 0;
            });
            scenes[order[0]].entities[order[1]].components.push(new_c); //component_ids[Object.keys(components).indexOf(component_type)]
            $("#obj_form")[0].reset();
            $("#new_obj").modal("toggle");
            updateMenu();
            
        }
        else alert("Entity " + entity.id + " already has a " + component_type + " component");
        $("#add_obj").off("click");
    });
    
}

function newCol() {
    return $("<div class='col'></div>");
}
function newFlex() { 
    return $("<div class='d-flex'></div>");
}

function newFlexItem(size=2) {
    return $("<div class='p-"+size+"'></div>");;
}

function makeid() {
    return Math.random().toString(36).substr(2, 5);
}