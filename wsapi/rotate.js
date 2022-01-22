    $("#logo").css("display","none");
    var canvas = document.createElement("canvas");
    canvas.style.width = "8em";
    canvas.style.height = "8em";
    canvas.id="nav_canv";
    $("#logo").parent().append(canvas);
    var ctx = canvas.getContext("2d");
    let w = canvas.width;
    let h = canvas.height;
    
    let ow = w/2;
    let oh = 0;
    let dw = 5;
    let dh = 0;
    let size = 30;
    function rgb(rs=0, gs=0, bs=0) {
        let r = Math.floor(Math.random() * 256);
        let g = Math.floor(Math.random() * 256);
        let b = Math.floor(Math.random() * 256);
        r+=rs; g+=gs; b+=bs;

        r = Math.min(Math.max(parseInt(r), 0), 255);
        g = Math.min(Math.max(parseInt(g), 0), 255);
        b = Math.min(Math.max(parseInt(b), 0), 255);

        return "rgb("+r+","+g+","+b+")";
    }
    let r=0, g=0, b=0;
    let loop = 0;
    function animate() {
        ctx.fillStyle = "rgb(20, 222, 5)";
        // ctx.lineWidth = "5px";
        ctx.beginPath();
        ctx.moveTo(w/2, h/2);
        ctx.lineTo(ow-(dw*size/4), oh-(dh*size/4));
        // ctx.lineTo(ow, oh);
        ctx.lineTo(ow+(dw*size/4), oh+(dh*size/4));
        ctx.closePath();
        ctx.fill();    
        if(ow>w) {
            ow=w
            dh=1;
            dw=0;
        }
        if(ow<0) {
            ow=0
            dh=-1;
            dw=0;
        }
        if(oh<0) {
            oh=0
            dw=1;
        }
        if(oh>h) {
            oh=h
            dh=0;
            dw=-1;
        }
        if(loop == 0 &&ow==w/2 && oh==0) {
            // let im = new Image();
            // im.width = canvas.width;
            // im.height = canvas.height;
            // im.onload = function() {
            //     $("#logo")[0].src = this.src;
            // }
            // // here is the most important part because if you dont replace you will get a DOM 18 exception.
            // im.src = canvas.toDataURL("image/png").replace("image/png", "image/octet-stream");    
            loop++;
        }
        ow+=size*dw;
        oh+=size*dh;
        
        
        setTimeout(function(){
            ctx.fillStyle = "black";
            ctx.fillRect(0,0,1000,1000);
            window.requestAnimationFrame(animate);
        },125);
    }
    window.requestAnimationFrame(animate);

