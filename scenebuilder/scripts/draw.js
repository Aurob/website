setSceneBounds(1000, 10, 1000);
function render() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    drawSceneBounds();
    let _textures = drawObjects();
    _textures.forEach(texture => {
        var img = new Image();
        img.src = texture[0];
        let position = texture[1];
        let shape = texture[2];
        ctx.drawImage(img, position.x + (width/2) - shape.w/2, position.z + (length/2) - shape.l/2, shape.w, shape.l);
    });
    window.requestAnimationFrame(render);
}
updateMenu();
render();

function setSceneBounds(w, h, l) {
    width = w;
    height = h;
    length = l;
    canvas.width = width;
    canvas.height = length;
}

function drawSceneBounds() {
    ctx.strokeStyle = "black";
    ctx.beginPath();
    ctx.rect(0, 0, width, length);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(width/2, 0);
    ctx.lineTo(width/2, length);
    ctx.moveTo(0, length/2);
    ctx.lineTo(width, length/2);
    ctx.stroke();
}

function drawObjects() {
    let _textures = [];
    for(let s = 0; s < scenes.length; s++) {
        let scene = JSON.parse(JSON.stringify(scenes[s]));
        scene.entities.sort(function(e1, e2) {                
            let e1pos = getValue(e1.components, "Position");
            let e1shape = getValue(e1.components, "Shape");
            let e2pos = getValue(e2.components, "Position");
            let e2shape = getValue(e2.components, "Shape");

            if(e1shape == undefined || e1pos == undefined) return -1;
            else if(e2shape == undefined || e2pos == undefined) return 1;
            else if(e1pos.y + (e1shape.h/2) > e2pos.y + (e2shape.h/2)) return 1;
            else if(e1pos.y + (e1shape.h/2) < e2pos.y + (e2shape.h/2)) return -1;
            return 0;
        });
        for(let e = 0; e < scene.entities.length; e++) {
            let entity = scene.entities[e];
            let position;
            let shape;
            let color;
            let texture;
            
            entity.components.forEach(component => {
                if(component.type == "Position" || component.type == 0) {
                    position = {"x":component.x, "y":component.y, "z":component.z};
                }
                if(component.type == "Shape" || component.type == 1) {
                    shape = {"w":component.w, "h":component.h, "l":component.l};
                }
                if(component.type == "Color" || component.type == 7) {
                    color = {"r":component.r, "g":component.g, "b":component.b};
                }
                if(component.type == "Texture" || component.type == 8) {
                    texture = component.url;
                }
            });
            
            
            if(position) {
                
                if(!shape) shape = {"w":10, "l":10};

                if(selected && selected.id == entity.id) ctx.strokeStyle = "red";
                else ctx.strokeStyle = "black";

                //Draw an outline around the entitiy position
                ctx.beginPath();
                ctx.rect(position.x + (width/2) - shape.w/2, position.z + (length/2) - shape.l/2, shape.w, shape.l);
                ctx.stroke();
                
                if(color) {
                    ctx.fillStyle = "rgb("+color.r +","+ color.g + "," + color.b +")";
                    ctx.fillRect(position.x + (width/2) - shape.w/2, position.z + (length/2) - shape.l/2, shape.w, shape.l);
                }
                if(texture) {
                    _textures.push([texture, position, shape]);
                }

                if(show_labels) {
                    
                    ctx.fillStyle = "black";
                    let tx = position.x + (width/2) - shape.w/2;
                    let ty = (position.z + (length/2)) - (shape.l/2.5);
                    ctx.fillText(entity.id + " - " + scene.id, tx, ty);
                    ctx.fillText("x: " + position.x + " - w: " + shape.w, tx, ty+10);
                    ctx.fillText("y: " + position.y + " - h: " + shape.h, tx, ty+20);
                    ctx.fillText("z: " + position.z + " - l: " + shape.l, tx, ty+30);
                }
            }
        }
    }

    return _textures;
}