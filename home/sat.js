const MinLatitude = -85.05112878;  
const MaxLatitude = 85.05112878;  
const MinLongitude = -180;  
const MaxLongitude = 180; 
            
var im_width = document.getElementsByTagName("body")[0].scrollWidth;
var im_height = document.getElementsByTagName("body")[0].scrollHeight;
var im_zoom = 9;



let main_canvas = document.getElementById("cvbg");
let main_ctx = main_canvas.getContext("2d");

var global_coord = {};

let key="AlVUTrENvRYkOVi5K3PVdyxritBA0U6Vt2hfTuqhBVqOaGo9PRDfckJN94LRjQD1";

function gotoEarth() {
    let link = "https://earth.google.com/web/@"+global_coord.value[0]+","+global_coord.value[1]+",11937739d";
    window.open(link);
}
function generate_sat_bg(){
    let width = im_width;
    
    let height = im_height;
    let scale = im_zoom;

    let xTiles = Math.ceil(width/256);
    let yTiles = Math.ceil(height/256);
                    
    let zoom = scale;
    
    function generate_image(coord, response){
        global_coord.value = coord;
        let pixel = coord_to_pixel(coord[0], coord[1], zoom);
        var tile = pixel_to_tile(pixel[0], pixel[1]);
        console.log(pixel);
        console.log(tile);
        for(let y = 0; y <yTiles; y++){  
            for(let x = 0; x < xTiles; x++){
                new_tile = [parseInt(tile[0]+x), parseInt(tile[1]+y)];
                qkey = tile_to_qkey(new_tile[0], new_tile[1], zoom);

                let new_img = new Image();
                new_img.onload = ()=>{
                    main_ctx.drawImage(new_img, x*256, y*256);
                    // if(response){
                    //     loc.innerHTML = response.resourceSets[0].resources[0].name;
                    //     point.innerHTML = response.resourceSets[0].resources[0].point.coordinates;
                    // }
                };
                new_img.src = get_url(qkey.join(''));   
            }
        }

        main_canvas.setAttribute('width', width);
        main_canvas.setAttribute('height', height);
        if(main_canvas.width > window.innerWidth || main_canvas.height > window.innerHeight){
            main_canvas.style.width = window.innerWidth+"px";
        }
    }
    // if(keep_loc.checked){
    //     generate_image(global_coord.value, false);
    // }
    // else{
    let coord = [rand(MinLatitude, MaxLatitude)+Math.random(), rand(MinLongitude, MaxLongitude)+Math.random()];
    let valid = [];
    let url = "https://dev.virtualearth.net/REST/v1/Locations/"
            +coord[0]+','+coord[1]
            +"?o=json&key="+key
    let xml = new XMLHttpRequest();
    xml.open("GET", url, true);
    xml.setRequestHeader('Content-type', 'application/json'); //URL encoded form
    xml.onreadystatechange = ()=>{
        if(xml.readyState == XMLHttpRequest.DONE){
            if(xml.response){
                let response = JSON.parse(xml.response);
                if(response.resourceSets[0].estimatedTotal > 0){
                    let location = response.resourceSets[0].resources[0];
                    console.log(location);
                    if(location.name != "Antarctica"){
                        // $("#sat_info").html(JSON.stringify(location.address));
                        $("#country").html("<li>"+location.address.countryRegion+"</li>");
                        generate_image(coord, response);
                    }else{
                        generate_sat_bg();
                    }
                }else{
                    generate_sat_bg();
                }
            }else{
                generate_sat_bg();
            }
        }
    }
    xml.send(null);
}

function rand(min, max){
    return Math.floor(Math.random() * (max - min + 1) + min);
}
function clip(n,min,max){
    return Math.min(Math.max(n, min), max);
}
function mapsize(zoom){
    return 256 << zoom;
}
function coord_to_pixel(lat, lon, zoom){
    lat = clip(lat, MinLatitude, MaxLatitude);
    lon = clip(lon, MinLongitude, MaxLongitude);

    let x = (lon + 180) / 360;
    let sinlat = Math.sin(lat * Math.PI / 180);
    let y = 0.5 - Math.log((1 + sinlat) / (1 - sinlat)) / (4 * Math.PI);

    let msize = mapsize(zoom);
    let pixelX = clip(x * msize + 0.5, 0, msize -1);
    let pixelY = clip(y * msize + 0.5, 0, msize -1);
    return [pixelX, pixelY]
}

function pixel_to_tile(x, y){
    return [x/256, y/256];
}

function tile_to_qkey(x, y, zoom){
    let qkey = [];
    for(i = zoom; i > 0; i--){
        digit = "0";
        mask = 1 << (i - 1);
        if((x & mask) != 0){
            digit++;
        }
        if((y&mask) != 0){
            digit++;
            digit++;
        }
        qkey.push(parseInt(digit));
    }
    return qkey;
}

function get_url(qkey){
    return "https://ecn.t2.tiles.virtualearth.net/tiles/a"+qkey+"?g=1&";
}         