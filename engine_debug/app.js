var express = require('express')
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var port = process.env.PORT || 1234;

app.use("/engine_debug", express.static(__dirname));
app.get('/*', function(req, res){
    console.log("Loading...");
    res.sendFile('index.html', { root: __dirname });
});

function onConnect(socket){
  console.log(true);
  socket.on("client_update", (data)=>{
    console.log(data);
  });
}
io.on('connection',onConnect);
http.listen(port, () => console.log('listening on '+port));

