<?PHP
  function test(){
  //   $img = file_get_contents( 
  //     $img_url
  //   ); 
      
  //   // Encode the image string data into base64 
  //   $data = base64_encode($img); 
      
  //   // Display the output 
  //   return $data; 
    return $_COOKIE["test"];
   }
?>
<script>
  document.cookie = escape("test") + "=" + new Date().getTime() + ";";
  function doit(){
    alert("<?php echo $_COOKIE["test"] ?>");
  }

</script>
<button onclick="doit()"></button>

<!-- <!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <style>
        canvas{
            border: solid;
        }
    </style>
    </head>
    <body>
        <table>
            <tbody>
                <tr><td><canvas></canvas></td></tr>
                <tr>
                    Width: <input type="number" min=256 max=4096 value=256 id="width">
                    Height: <input type="number" min=256 max=4096 value=256 id="height">
                    Zoom: <input type="number" min=1 max=20 value=4 id="zoom">
                    <button onclick="generate()">Submit</button>
                </tr>
                <tr><td>Exclude Antarctica: <input id="no_ant" type="checkbox"></td></tr>
                <tr><td id="name"></td></tr>
                <tr><td id="coord"></td></tr>
            </tbody>
        </table>
        <script>
            //Bing Tiles
            const MinLatitude = -85.05112878;  
            const MaxLatitude = 85.05112878;  
            const MinLongitude = -180;  
            const MaxLongitude = 180; 
                        
            var loc = document.getElementById("name");
            var point = document.getElementById("coord");
            var no_ant = document.getElementById("no_ant");
            var im_width = document.getElementById("width");
            var im_height = document.getElementById("height");
            var im_zoom = document.getElementById("zoom");

            

            let main_canvas = document.getElementsByTagName("canvas")[0];
            let main_ctx = main_canvas.getContext("2d");
            
            let key="AlVUTrENvRYkOVi5K3PVdyxritBA0U6Vt2hfTuqhBVqOaGo9PRDfckJN94LRjQD1";
            
            function generate(){
                let width = im_width.value;
                let height = im_height.value;
                let scale = im_zoom.value;

                if(width < 256) width = 256;
                if(width > 4096) width = 4096;

                if(height < 256) height = 256;
                if(height > 4096) height = 4096;

                if(scale < 1) scale = 1;
                if(scale > 20) scale = 20;

                let xTiles = Math.ceil(width/256);
                let yTiles = Math.ceil(height/256);
                main_canvas.setAttribute('width', width);
                main_canvas.setAttribute('height', height);
                
                let zoom = scale;

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
                        let response = JSON.parse(xml.response);
                        if(response.resourceSets[0].estimatedTotal > 0){
                            if(!no_ant.checked || no_ant.checked && response.resourceSets[0].resources[0].name != "Antarctica"){
                                let pixel = coord_to_pixel(coord[0], coord[1], zoom);
                                var tile = pixel_to_tile(pixel[0], pixel[1]);

                                let i = 0;
                                let ims = [];
                                for(let y = 0; y <yTiles; y++){
                                    
                                    for(let x = 0; x < xTiles; x++){
                                        new_tile = [parseInt(tile[0]+x), parseInt(tile[1]+y)];
                                        qkey = tile_to_qkey(new_tile[0], new_tile[1], zoom);

                                        let new_img = new Image();
                                        new_img.onload = ()=>{
                                            ims[i] = new_img;
                                            main_ctx.drawImage(new_img, x*256, y*256)
                                            loc.innerHTML = response.resourceSets[0].resources[0].name;
                                            point.innerHTML = response.resourceSets[0].resources[0].point.coordinates;
                                        };
                                        let url = get_url(qkey.join(''));
                                        new_img.src = "<?php echo test(". url .") ?>";   
                                        i++;
                                    }
                                }
                            
                                var x = 0;
                                var y = 0;
                                for(let i = 0; i < ims.length; i++){
                                    let image = ims[i];
                                    main_ctx.drawImage(image, x, y);
                                }
                            }else{
                                generate();
                            }
                        }else{
                            generate();
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
        </script>
    </body>
</html> -->