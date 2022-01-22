var Components = {"Position":0, "Shape":1, "Color":7, "Interaction":3, "Texture":8, "Teleport":4, "Model":9};
var Components2 = {0:"Position", 1:"Shape", 3:"Interaction", 7:"Color", 8:"Texture", 4:"Teleport", 9:"Model"};
var components = {
    "Scene" : [
        ["id"], ["name"]
    ],
    "Position": [
        ["x", "y", "z"], []
    ],
    "Shape": [
        ["w", "h", "l"], []
    ],
    "Color":[
        ["r", "g", "b"], []
    ],
    "Interaction": [
        [], ["action"]
    ],
    "Transition": [
        ["to_scene"], []
    ],
    "Teleport": [
        ["x", "y", "z"], []
    ],
    "Texture" : [
        ["file_name", "url"], []
    ],
    "Model" : [
        ["file_name"], ["scale"]
    ]
};

var component_ids = [
    0, 1, 2, 7, 3, 4, 5
]
var scenes = [
    {
        "id" : "abc",
        "entities" : [
            {
                "id" : "def",
                "components" : [
                    {
                        "id" : "ghi",
                        "type" : "Position",
                        "x":0, "y": 0, "z": 0
                    },
                    {
                        "id" : "jkl",
                        "type" : "Shape",
                        "w":100, "h": 100, "l": 100
                    }
                ]
            }
        ]
    }
];

var undo_history = [];
var redo_history = [];
var texture_urls = [];
var event_ids = [];
var show_labels = true;
var width, height, length;
var selected;
var mousedown = false;
var mousex, mousey;
var last_mousex, last_mousey;
var resize_w;
var resize_l;
var resizing = false;

var canvas = $("#canvas")[0];
var ctx = canvas.getContext("2d");
ctx.font = "30px Arial";



$(document).ready(function(){
    //Populate the new component selector with each defined component
    let scene_query = new URLSearchParams(window.location.search)
    if(scene_query.has("scene")) {
        fetch("/test/resources/scenes/"+scene_query.get("scene")+".json")
        .then(data=>data.json())
        .then(json=>{loadScene(json)});
    }
    $("#component_list").append($('<option></option>')); //add initial blank option
    Object.keys(components).forEach(component => {
        $("#component_list").append($('<option></option>').val(component).text(component));
    });
});

function loadScene(scene_json) {
    try {
        let temp_scenes;
        if(typeof(scene_json) != "object")
            temp_scenes = JSON.parse(scene_json);
        else temp_scenes = scene_json;
        if("Scenes" in temp_scenes) scenes = temp_scenes["Scenes"];
        else throw "Invalid Configuration";

        for(let s = 0; s < scenes.length; s++) {
            let scene = scenes[s];
            for(let e = 0; e < scene.entities.length; e++) {
                let entity = scene.entities[e];
                for(let c = 0; c < entity.components.length; c++) {
                    let component = entity.components[c];
                    if(!isNaN(component.type)) scenes[s].entities[e].components[c].type = Components2[component.type];
                }
            }
        }
        updateMenu();
    }
    catch(er) {
        console.log(er);
        alert("Invalid configuration");
    }
}


function undo() {
    if(undo_history.length > 0) {
        redo_history.push(entities);
        entities = undo_history.pop();
        updateMenu();
    }
}

function redo() {
    if(redo_history.length > 0) {
        undo_history.push(entities);
        entities = redo_history.pop();
        updateMenu();
    }
}



function z_indexEntities(entity1, entity2) {
    console.log(123);
}


function hexToRgb(hex) {
    // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
    var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
    hex = hex.replace(shorthandRegex, function(m, r, g, b) {
      return r + r + g + g + b + b;
    });
  
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16)
    } : null;
}

function getIndex(type, id) {
    
}

function getValue(components, key) {
    
    for(let c = 0; c < components.length; c++) {
        let component = components[c];    
        if(component.type == key) {
            return component;
        } 
    }    
}

function getOrder(id) {
    for(let s = 0; s < scenes.length; s++) {
        let scene = scenes[s];
        if(scene.id == id) return [s];
        
        for(let e = 0; e < scene.entities.length; e++) {
            let entity = scene.entities[e];
            if(entity.id == id) return [s, e];
            
            for(let c = 0; c < entity.components.length; c++) {
                let component = entity.components[c];
                if(component.id == id) return [s, e, c];
            }
        }
    }
}