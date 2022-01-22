function set_image(image){
                //drop_ctx.clearRect(0, 0, drop.width, drop.height);
                var new_image = new Image();
                new_image.onload = function() {
                    hidden.setAttribute('width', new_image.width);
                    hidden.setAttribute('height', new_image.height);
                    hidden_ctx.drawImage(new_image, 0, 0);
                    drop.src = image;
                };
                new_image.src = image;
            }